package nautilus.writingpane;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import android.graphics.drawable.ColorDrawable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

/**
 * Created by davu on 2/29/2016.
 */
public class WritingPane extends View {
	
	private static final float STROKE_WIDTH = 5f;
	/** Need to track this so the dirty region can accommodate the stroke. **/
	private static final float HALF_STROKE_WIDTH = STROKE_WIDTH / 2;

	private Paint mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
	private Path mPath = new Path();

	  /**
	   * Optimizes painting by invalidating the smallest possible area.
	   */
  
	private final RectF mDirtyRect = new RectF();
	private float mLastDownX;
	private float mLastDownY;
	private float mPreX;
	private float mPreY;
	private int mColor;
	private boolean mClearScreen = false;

	private boolean mDrawBitmap = false;
	private Bitmap mBitmap = null;
	private Canvas mCanvas = null;
	
    public WritingPane(Context ctx) {
        super(ctx);
		init();
    }

    public WritingPane(Context context, AttributeSet attrs) {
        super(context, attrs);
		init();
    }

    public WritingPane(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
		init();
    }
	
	private void init() {

		mPaint.setColor(Color.RED);
		mPaint.setAntiAlias(true);
		mPaint.setStyle(Paint.Style.STROKE);
		mPaint.setStrokeJoin(Paint.Join.ROUND);
		mPaint.setStrokeWidth(STROKE_WIDTH * 2);
	}

	@Override
	protected void onMeasure(int widthSpec, int heightSpec) {
		final int intrinsicSize = 100;
		int widthMode = MeasureSpec.getMode(widthSpec);
		int widthSize = MeasureSpec.getSize(widthSpec);
		int heightMode = MeasureSpec.getMode(heightSpec);
		int heightSize = MeasureSpec.getSize(heightSpec);
		int width;
		int height;
		
		if (widthMode == MeasureSpec.EXACTLY) {
			width = widthSize;
		} else if (widthMode == MeasureSpec.AT_MOST) {
			width = Math.min(intrinsicSize, widthSize);
		} else {
			width = intrinsicSize;
		}

		if (heightMode == MeasureSpec.EXACTLY) {
			height = heightSize;
		} else if (heightMode == MeasureSpec.AT_MOST) {
			height = Math.min(intrinsicSize, heightSize);
		} else {
			height = intrinsicSize;
		}
		
		setMeasuredDimension(width, height);

		if(mBitmap != null) {
			mBitmap.recycle();
		}

		mBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
		mCanvas = new Canvas(mBitmap);
	}

	/**
	 * Draw this view to a bitmap and return it. The function takes time.
	 * @return
	 */
	public Bitmap getBitmap() {
		Bitmap result;
		setDrawingCacheEnabled(true);
		result = Bitmap.createBitmap(getDrawingCache());
		setDrawingCacheEnabled(false);
		return result;
	}

	public void clear() {
		mClearScreen = true;
		mDrawBitmap = false;
		mPath.reset();
		invalidate();
	}

	public void dispose() {
		if(mBitmap != null) {
			mBitmap.recycle();
		}
	}

	public void drawBitmap(final Bitmap bmp) {
		mClearScreen = true;
		mDrawBitmap = true;
		mCanvas.drawBitmap(bmp, 0, 0, mPaint);
		invalidate();
	}

    @Override
    protected void onDraw(Canvas canvas) {
		if(mClearScreen) {
			canvas.drawColor(((ColorDrawable)getBackground()).getColor());
			if(mDrawBitmap && (mBitmap != null)) {
				canvas.drawBitmap(mBitmap, 0, 0, mPaint);
			}
			mClearScreen = false;
		} else {
			canvas.drawPath(mPath, mPaint);
		}
    }

	@Override
	public boolean onTouchEvent(MotionEvent ev) {
		int i, historySize;
		float historicalX;
		float historicalY;
		float eX = ev.getX();
		float eY = ev.getY();

		switch(ev.getAction()) {
			case MotionEvent.ACTION_DOWN:
				mPath.moveTo(eX, eY);
				mLastDownX = eX;
				mLastDownY = eY;
				break;

			case MotionEvent.ACTION_MOVE:
				// Start tracking the dirty region.
				resetDirtyRect(eX, eY);

				// When the hardware tracks events faster than they are delivered, the
				// event will contain a history of those skipped points.
				historySize = ev.getHistorySize();
				for (i = 0; i < historySize; i++) {
					historicalX = ev.getHistoricalX(i);
					historicalY = ev.getHistoricalY(i);
					expandDirtyRect(historicalX, historicalY);
					mPath.lineTo(historicalX, historicalY);
				}
				// After replaying history, connect the line to the touch point.
				mPath.lineTo(eX, eY);
				break;

			case MotionEvent.ACTION_UP:
				// Start tracking the dirty region.
				resetDirtyRect(eX, eY);

				// When the hardware tracks events faster than they are delivered, the
				// event will contain a history of those skipped points.
				historySize = ev.getHistorySize();
				for (i = 0; i < historySize; i++) {
					historicalX = ev.getHistoricalX(i);
					historicalY = ev.getHistoricalY(i);
					expandDirtyRect(historicalX, historicalY);
					mPath.lineTo(historicalX, historicalY);
				}
				// After replaying history, connect the line to the touch point.
				mPath.lineTo(eX, eY);
				break;

//			case MotionEvent.ACTION_CANCEL:
//				break;
			default:
				return false;
		}
		invalidate(
				(int) (mDirtyRect.left - HALF_STROKE_WIDTH),
				(int) (mDirtyRect.top - HALF_STROKE_WIDTH),
				(int) (mDirtyRect.right + HALF_STROKE_WIDTH),
				(int) (mDirtyRect.bottom + HALF_STROKE_WIDTH));
			
		mPreX = eX;
		mPreY = eY;
		
		return true;
	}
	
	/**
   * Called when replaying history to ensure the dirty region includes all
   * points.
   */
	private void expandDirtyRect(float historicalX, float historicalY) {
		if (historicalX < mDirtyRect.left) {
			mDirtyRect.left = historicalX;
		} else if (historicalX > mDirtyRect.right) {
			mDirtyRect.right = historicalX;
		}
		if (historicalY < mDirtyRect.top) {
			mDirtyRect.top = historicalY;
		} else if (historicalY > mDirtyRect.bottom) {
			mDirtyRect.bottom = historicalY;
		}
	}

	/**
	* Resets the dirty region when the motion event occurs.
	*/
	private void resetDirtyRect(float eventX, float eventY) {
		// The mLastDownX, mLastDownY were set when the ACTION_DOWN
		// motion event occurred.
		mDirtyRect.left = Math.min(mLastDownX, eventX);
		mDirtyRect.right = Math.max(mLastDownX, eventX);
		mDirtyRect.top = Math.min(mLastDownY, eventY);
		mDirtyRect.bottom = Math.max(mLastDownY, eventY);
	}
}

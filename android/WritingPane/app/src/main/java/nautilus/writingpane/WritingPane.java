package nautilus.writingpane;

import android.content.Context;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.View;

/**
 * Created by davu on 2/29/2016.
 */
public class WritingPane extends View {
	
	private static final float STROKE_WIDTH = 5f;
	/** Need to track this so the dirty region can accommodate the stroke. **/
	private static final float HALF_STROKE_WIDTH = STROKE_WIDTH / 2;

	private Paint mPaint = new Paint();
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
	}
	
	@Override
	protected void onMeasure(int widthSpec, int heightSpec) {
		final int intrinsicSize = 100;
		int widthMode = MeasureSpec.getMode(widthMeasureSpec);
		int widthSize = MeasureSpec.getSize(widthMeasureSpec);
		int heightMode = MeasureSpec.getMode(heightMeasureSpec);
		int heightSize = MeasureSpec.getSize(heightMeasureSpec);
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
	}

    @Override
    protected void onDraw(Canvas canvas) {
		canvas.drawPath(mPath, mPaint);
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
				path.moveTo(eX, eY);
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
				path.lineTo(eventX, eventY)
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
				path.lineTo(eventX, eventY)
				
				//Xu ly
			break;
			
			case MotionEvent.ACTION_CANCEL:
			break;
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

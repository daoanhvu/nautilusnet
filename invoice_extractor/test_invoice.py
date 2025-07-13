from transformers import pipeline
from PIL import Image

# Initialize the document-question-answering pipeline with the specific model
# The 'impira/layoutlm-invoices' model is designed for document QA.
pipe = pipeline("document-question-answering",
                model="impira/layoutlm-invoices")

# --- Example 2: Using a local image file ---
# For this example to run, you'd need a local invoice image.
# Let's assume you have an image named 'my_invoice.png' in the same directory.
# You can replace this with any path to your local invoice image.
try:
    # Replace with your actual image path
    local_image_path = "./data/invoice-001.png"
    # Create a dummy image for demonstration if 'my_invoice.png' doesn't exist
    try:
        Image.new('RGB', (600, 800), color='white').save(local_image_path)
        print(f"Dummy image '{local_image_path}' created for demonstration.")
        # Note: A dummy white image won't give meaningful answers,
        # you'll need a real invoice image for proper results.
    except Exception as e:
        print(
            f"Could not create dummy image: {e}. Please provide a real image at '{local_image_path}' for this part of the example.")

    image = Image.open(local_image_path).convert("RGB")

    print(f"Querying local image: {local_image_path}")
    question4 = "Who is the recipient?"
    answer4 = pipe(image=image, question=question4)

    print(f"Question: {question4} -> Answer: {answer4}")

except FileNotFoundError:
    print(f"Error: '{local_image_path}' not found. Please make sure you have an invoice image at this path or replace it with a valid path.")
except Exception as e:
    print(f"An error occurred while processing the local image: {e}")

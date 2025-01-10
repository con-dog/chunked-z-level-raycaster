import os
from PIL import Image

def calculate_average_color(image_path):
    with Image.open(image_path) as img:
        img = img.convert("RGB")
        pixels = list(img.getdata())
        num_pixels = len(pixels)

        r_total, g_total, b_total = 0, 0, 0

        for r, g, b in pixels:
            r_total += r
            g_total += g
            b_total += b

        avg_r = r_total // num_pixels
        avg_g = g_total // num_pixels
        avg_b = b_total // num_pixels

        return f"#{avg_r:02x}{avg_g:02x}{avg_b:02x}"

def process_directory(directory):
    for root, _, files in os.walk(directory):
        for file in files:
            if file.lower().endswith(".png"):
                file_path = os.path.join(root, file)
                relative_path = os.path.relpath(file_path, directory)
                avg_hex_color = calculate_average_color(file_path)
                print(f"{relative_path}: {avg_hex_color}")

if __name__ == "__main__":
    directory = input("Enter the path to the directory: ").strip()
    process_directory(directory)

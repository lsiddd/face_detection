
# Face Detection with OpenCV

This project is a simple face detection application using OpenCV and C++. It processes images in a directory (and its subdirectories) to detect faces using the Haar Cascade Classifier. Detected faces are either displayed or saved to a specified directory.

## Features
- **Face Detection**: Uses OpenCV’s pre-trained Haar Cascade model (`haarcascade_frontalface_default.xml`) to detect faces in images.
- **Image Processing**: Resizes images to maintain aspect ratio and applies noise reduction techniques (e.g., Gaussian Blur, Bilateral Filter).
- **Optional Image Saving**: Detected faces can be saved to a specified directory, or simply displayed on the screen.
- **Recursive Directory Processing**: The application can process images in a given directory and its subdirectories.

## Prerequisites

### Software Requirements:
- **C++17** or later
- **OpenCV 4** or later

### Libraries:
- OpenCV (for image processing and face detection)
- C++17 standard library for filesystem and algorithm utilities

## Installation

### Step 1: Install OpenCV

Follow the [official OpenCV installation guide](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html) for your platform.

### Step 2: Clone the Repository

```bash
git clone https://github.com/your-username/face-detection.git
cd face-detection
```

### Step 3: Setup the Project

You can use **Meson** to build the project.

1. **Install Meson and Ninja** (if not already installed):
   - On Ubuntu/Debian:
     ```bash
     sudo apt install meson ninja-build
     ```
   - On macOS (via Homebrew):
     ```bash
     brew install meson ninja
     ```

2. **Create build directory and build**:
   ```bash
   meson setup build
   meson compile -C build
   ```

3. **Run the project**:
   - After building, the executable will be located in the `build` directory.
   - You can run the program by specifying a directory to process. Optionally, you can specify a directory to save processed images.

## Usage

```bash
./build/face_detection <directory_path> [--save <save_directory>]
```

- `<directory_path>`: The path to the directory containing images to process (can be a relative or absolute path).
- `--save <save_directory>`: Optional flag to save the processed images (with detected faces) to the specified directory.

### Example 1: Process images and display detected faces
```bash
./build/face_detection ./images/
```

### Example 2: Process images and save them with detected faces
```bash
./build/face_detection ./images/ --save ./output_images/
```

## How It Works

1. **Haar Cascade Classifier**: The application uses OpenCV’s pre-trained `haarcascade_frontalface_default.xml` model for detecting faces in images. This model is loaded at the start of the program.
2. **Processing Directory**: The program recursively processes all image files in the specified directory and its subdirectories.
3. **Face Detection**: For each image, the program detects faces and draws rectangles around them.
4. **Optional Saving**: If the `--save` flag is specified, the processed images are saved to the provided directory. Otherwise, the images with detected faces are displayed one by one.

## File Structure

```
.
├── haarcascade_frontalface_default.xml  # Pre-trained Haar Cascade model for face detection
├── meson.build                         # Meson build configuration
├── src/                                 # Source files
│   └── main.cpp                         # Main C++ source file
└── README.md                            # This README file
```

## Troubleshooting

- **Haar Cascade Model Not Found**: Ensure that the file `haarcascade_frontalface_default.xml` is in the project root directory, or adjust the code to point to the correct path.
- **OpenCV Installation Issues**: If you encounter issues related to OpenCV, make sure you have installed it correctly and linked it to your project during the build process.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


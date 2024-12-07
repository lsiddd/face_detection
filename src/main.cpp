#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <algorithm>

#define LOG(x) std::cout << x << std::endl

namespace fs = std::filesystem;

// Supported image file extensions
const std::vector<std::string> IMAGE_EXTENSIONS = {".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff"};

// Check if the file is an image based on its extension (case-insensitive)
bool IsImageFile(const fs::path& filePath)
{
    std::string extension = filePath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return std::find(IMAGE_EXTENSIONS.begin(), IMAGE_EXTENSIONS.end(), extension) != IMAGE_EXTENSIONS.end();
}

// Resize image while maintaining aspect ratio, with a max height of maxHeight
cv::Mat ResizeImageWithMaxHeight(const cv::Mat& image, int maxHeight)
{
    int originalWidth = image.cols;
    int originalHeight = image.rows;

    float scaleFactor = static_cast<float>(maxHeight) / originalHeight;

    if (scaleFactor < 1.0f)
    {
        int newWidth = static_cast<int>(originalWidth * scaleFactor);
        cv::Size newSize(newWidth, maxHeight);

        cv::Mat resizedImage;
        cv::resize(image, resizedImage, newSize, 0, 0, cv::INTER_AREA);
        return resizedImage;
    }

    return image;
}

void DetectFaces(const cv::Mat& image, cv::CascadeClassifier& faceCascade, std::vector<cv::Rect>& detectedFaces)
{
    // Convert image to grayscale
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    int minResolution = std::min(image.cols, image.rows);
    int filterSize = std::max(60, minResolution / 10);

    // Apply Gaussian Blur to reduce noise
    cv::GaussianBlur(grayImage, grayImage, cv::Size(5, 5), 0);

    // Equalize the histogram to improve contrast
    cv::equalizeHist(grayImage, grayImage);

    // Apply bilateral filter to preserve edges while reducing noise
    cv::Mat filteredImage;
    cv::bilateralFilter(grayImage, filteredImage, 9, 75, 75);

    // Detect faces
    faceCascade.detectMultiScale(
        filteredImage,
        detectedFaces,
        1.1,                     // scaleFactor
        10,                      // minNeighbors
        cv::CASCADE_SCALE_IMAGE, // flags
        cv::Size(filterSize, filterSize) // minSize
    );

    // Remove overlapping detections using Non-Maximum Suppression (NMS)
    std::vector<cv::Rect> nonOverlappingFaces;
    for (const auto& face : detectedFaces)
    {
        bool isOverlapping = false;
        for (const auto& existingFace : nonOverlappingFaces)
        {
            float intersectionArea = (face & existingFace).area();
            float minArea = std::min(static_cast<float>(face.area()), static_cast<float>(existingFace.area()));
            if (intersectionArea / minArea > 0.3f) // Overlap threshold
            {
                isOverlapping = true;
                break;
            }
        }
        if (!isOverlapping)
        {
            nonOverlappingFaces.push_back(face);
        }
    }
    detectedFaces = nonOverlappingFaces;
}

// Process files in a directory recursively
void ProcessDirectory(const fs::path& directoryPath, cv::CascadeClassifier& faceCascade, const fs::path& saveDirectory, bool shouldSave)
{
    try
    {
        for (const auto& entry : fs::recursive_directory_iterator(directoryPath))
        {
            if (fs::is_regular_file(entry))
            {
                const fs::path& filePath = entry.path();

                if (IsImageFile(filePath))
                {
                    LOG("Processing image: " << filePath);

                    cv::Mat image = cv::imread(filePath.string());
                    if (image.empty())
                    {
                        std::cerr << "Could not open or find the image: " << filePath << std::endl;
                        continue;
                    }

                    std::vector<cv::Rect> faces;
                    DetectFaces(image, faceCascade, faces);

                    if (!faces.empty())
                    {
                        LOG("Faces detected: " << faces.size());

                        for (const auto& face : faces)
                        {
                            std::cout << "Face at: x=" << face.x
                                      << ", y=" << face.y
                                      << ", width=" << face.width
                                      << ", height=" << face.height << std::endl;

                            // Draw rectangles around detected faces
                            cv::rectangle(image, face, cv::Scalar(255, 0, 0), 2);
                        }

                        if (shouldSave)
                        {
                            fs::create_directories(saveDirectory);
                            fs::path savePath = saveDirectory / filePath.filename();

                            if (!cv::imwrite(savePath.string(), image))
                            {
                                std::cerr << "Failed to save the image to: " << savePath << std::endl;
                            }
                            else
                            {
                                LOG("Saved processed image to: " << savePath);
                            }
                        }
                        else
                        {
                            // Display the image with detected faces
                            cv::imshow("Detected Faces", image);
                            std::cout << "Press any key to continue to the next image...\n";
                            cv::waitKey(0);
                        }
                    }
                    else
                    {
                        LOG("No faces detected.");
                    }
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error processing directory: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    // Validate command-line arguments
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <directory_path> [--save <save_directory>]\n";
        return EXIT_FAILURE;
    }

    fs::path inputDirectory = argv[1];
    fs::path saveDirectory;
    bool saveImages = false;

    // Parse optional --save flag
    if (argc >= 3 && std::string(argv[2]) == "--save")
    {
        if (argc < 4)
        {
            std::cerr << "Error: Save directory not specified.\n";
            return EXIT_FAILURE;
        }
        saveDirectory = argv[3];
        saveImages = true;
    }

    // Check if the input directory exists and is a directory
    if (!fs::exists(inputDirectory) || !fs::is_directory(inputDirectory))
    {
        std::cerr << "The provided path is not a valid directory.\n";
        return EXIT_FAILURE;
    }

    // Load the Haar Cascade classifier for face detection
    cv::CascadeClassifier faceCascade;
    const std::string cascadeFile = "haarcascade_frontalface_default.xml"; // Adjust the path if necessary

    if (!faceCascade.load(cv::samples::findFile(cascadeFile)))
    {
        std::cerr << "Error loading face cascade from: " << cascadeFile << std::endl;
        return EXIT_FAILURE;
    }

    // Start processing the directory
    ProcessDirectory(inputDirectory, faceCascade, saveDirectory, saveImages);

    LOG("Processing completed.");
    return EXIT_SUCCESS;
}

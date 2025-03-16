#pragma once

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <iostream>
#include <vector>

void LoadDICOM(const std::string& filename) {
    DcmFileFormat fileformat;
    const OFCondition status{ fileformat.loadFile(filename.c_str()) };

    if (!status.good()) {
        std::cerr << "Error: Cannot read DICOM file " << status.text() << '\n';
        return;
    }

    DcmDataset *dataset{ fileformat.getDataset() };
    
    const Uint8 *pixelData{ nullptr };
    dataset->findAndGetUint8Array(DCM_PixelData, pixelData);

    if (!pixelData) {
        std::cerr << "Error: No pixel data found in the DICOM file\n";
        return;
    }

    int width, height;
    dataset->findAndGetSint32(DCM_Rows, height);
    dataset->findAndGetSint32(DCM_Columns, width);

    //std::cout << "Loaded DICOM Image - Width: " << width << ", Height: " << height << '\n';

    std::vector<uint8_t> imageData(width * height);

    for (int i{ 0 }; i < width * height; ++i) {
        imageData[i] = static_cast<uint8_t>(pixelData[i] / 256); // Convert 16-bit to 8-bit
    }
}

// Use code like this in main.cpp:
    //std::string dicomFile = "your_image.dcm";
    //LoadDICOM(dicomFile);
    //return 0;


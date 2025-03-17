#pragma once

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <iostream>
#include <vector>

class DicomHandler
{
    public:
        DicomHandler() : m_textureID{ 0 }, m_width{ 0 }, m_height{ 0 } {}

        void loadDICOM(const std::string& filename) {
            DcmFileFormat fileformat;
            const OFCondition status{ fileformat.loadFile(filename.c_str()) };

            if (!status.good()) {
                std::cerr << "Error: Cannot read DICOM file " << status.text() << '\n';
                return;
            }

            DcmDataset *dataset{ fileformat.getDataset() };
            
            const Uint16 *pixelData{ nullptr };
            dataset->findAndGetUint16Array(DCM_PixelData, pixelData);

            if (!pixelData) {
                std::cerr << "Error: No pixel data found in the DICOM file\n";
                return;
            }

            Uint16 rows = 0, cols = 0;
            dataset->findAndGetUint16(DCM_Rows, rows);
            dataset->findAndGetUint16(DCM_Columns, cols);

            m_width = static_cast<int>(cols);
            m_height = static_cast<int>(rows);

            //std::cout << "Loaded DICOM Image - Width: " << width << ", Height: " << height << '\n';

            // Normalize data to 8-bit range (0 to 255)
            Uint16 minValue{ 65535 }, maxValue{ 0 };
            for (size_t i{ 0 }; i < static_cast<size_t>(m_width * m_height); ++i) {
                if (pixelData[i] < minValue) 
                    minValue = pixelData[i];
                if (pixelData[i] > maxValue)
                    maxValue = pixelData[i];
            }
            m_imageData.resize(m_width * m_height);
            for (size_t i{ 0 }; i < static_cast<size_t>(m_width * m_height); ++i) {
                m_imageData[i] = static_cast<uint8_t>(((pixelData[i] - minValue) / static_cast<float>(maxValue - minValue)) * 255);
            }

            // Handle OpenGL texture
            glGenTextures(1, &m_textureID);
            glBindTexture(GL_TEXTURE_2D, m_textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, m_imageData.data());

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        GLuint getTextureID() const { return m_textureID; }
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

    private:
        GLuint m_textureID;
        int m_width, m_height;
        std::vector<uint8_t> m_imageData;
};

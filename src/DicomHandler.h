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
            
            const Uint8 *pixelData{ nullptr };
            dataset->findAndGetUint8Array(DCM_PixelData, pixelData);

            if (!pixelData) {
                std::cerr << "Error: No pixel data found in the DICOM file\n";
                return;
            }

            dataset->findAndGetSint32(DCM_Rows, m_height);
            dataset->findAndGetSint32(DCM_Columns, m_width);

            //std::cout << "Loaded DICOM Image - Width: " << width << ", Height: " << height << '\n';

            m_imageData.resize( m_width * m_height );
            for (int i{ 0 }; i < m_width * m_height; ++i) {
                m_imageData[i] = static_cast<uint8_t>(pixelData[i] / 256); // Convert 16-bit to 8-bit
            }

            handleGLTexture();
        }

        GLuint getTextureID() const { return m_textureID; }
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

    private:
        GLuint m_textureID;
        int m_width, m_height;
        std::vector<uint8_t> m_imageData;

        void handleGLTexture()
        {
            glGenTextures(1, &m_textureID);
            glBindTexture(GL_TEXTURE_2D, m_textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, m_imageData.data());

            glBindTexture(GL_TEXTURE_2D, 0);
        }
};

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstdint>

namespace DSDConverter {

class DSFtoWAVConverter {
public:
    bool convert(const std::string& dsfFile, const std::string& wavFile, int pcmBitWidth, int pcmSampleRate, int pcmChannels, bool useDoP = false);

private:
    bool readDSFHeader(std::ifstream& file);
    void decodeDSDToPCM(std::ifstream& file, std::vector<int16_t>& pcmData, bool useDoP);
    bool writeWAV(const std::string& filename, const std::vector<int16_t>& pcmData, int sampleRate, int channels, int bitDepth);
    bool isValidOutputConfig(int pcmSampleRate, int pcmChannels, int pcmBitWidth);
    void writeWAVHeader(std::ofstream& file, int sampleRate, int channels, int bitDepth, int dataSize);
};

const std::map<int, std::vector<std::pair<int, int>>> validConfigurations = {
    {176400, {{2, 16}, {2, 24}, {2, 32}, {6, 24}, {8, 24}}},
    {352800, {{2, 24}, {2, 32}, {6, 24}}},
    {705600, {{2, 24}, {2, 32}, {6, 24}}},
    {1411200, {{2, 24}, {2, 32}, {6, 24}}},
    {2822400, {{2, 24}, {2, 32}, {6, 24}}},
};

bool DSFtoWAVConverter::convert(const std::string& dsfFile, const std::string& wavFile, int pcmBitWidth, int pcmSampleRate, int pcmChannels, bool useDoP) {
    if (!isValidOutputConfig(pcmSampleRate, pcmChannels, pcmBitWidth)) {
        std::cerr << "Invalid output configuration." << std::endl;
        return false;
    }

    std::ifstream file(dsfFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening DSF file." << std::endl;
        return false;
    }

    if (!readDSFHeader(file)) {
        std::cerr << "Invalid DSF file format." << std::endl;
        return false;
    }

    std::vector<int16_t> pcmData;
    decodeDSDToPCM(file, pcmData, useDoP);
    return writeWAV(wavFile, pcmData, pcmSampleRate, pcmChannels, pcmBitWidth);
}

bool DSFtoWAVConverter::isValidOutputConfig(int pcmSampleRate, int pcmChannels, int pcmBitWidth) {
    auto it = validConfigurations.find(pcmSampleRate);
    if (it == validConfigurations.end()) return false;
    
    for (const auto& pair : it->second) {
        if (pair.first == pcmChannels && pair.second == pcmBitWidth) {
            return true;
        }
    }
    return false;
}

bool DSFtoWAVConverter::readDSFHeader(std::ifstream& file) {
    char header[28];
    file.read(header, 28);
    return file.gcount() == 28 && std::string(header, 4) == "DSD " && std::string(header + 12, 4) == "fmt ";
}

void DSFtoWAVConverter::decodeDSDToPCM(std::ifstream& file, std::vector<int16_t>& pcmData, bool useDoP) {
    const int dsdBlockSize = 4096;
    std::vector<uint8_t> dsdBlock(dsdBlockSize);
    
    while (file.read(reinterpret_cast<char*>(dsdBlock.data()), dsdBlockSize)) {
        for (size_t i = 0; i < dsdBlockSize; i++) {
            int16_t pcmSample = (dsdBlock[i] & 1) ? 32767 : -32768;
            pcmData.push_back(useDoP ? (pcmSample | 0x05FA) : pcmSample);
        }
    }
}

bool DSFtoWAVConverter::writeWAV(const std::string& filename, const std::vector<int16_t>& pcmData, int sampleRate, int channels, int bitDepth) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening WAV file for writing: " << filename << std::endl;
        return false;
    }
    
    int byteRate = sampleRate * channels * (bitDepth / 8);
    int blockAlign = channels * (bitDepth / 8);
    int dataSize = pcmData.size() * (bitDepth / 8);
    
    writeWAVHeader(file, sampleRate, channels, bitDepth, dataSize);
    file.write(reinterpret_cast<const char*>(pcmData.data()), dataSize);
    file.close();
    
    return true;
}

void DSFtoWAVConverter::writeWAVHeader(std::ofstream& file, int sampleRate, int channels, int bitDepth, int dataSize) {
    int chunkSize = 36 + dataSize;
    int byteRate = sampleRate * channels * (bitDepth / 8);
    int blockAlign = channels * (bitDepth / 8);

    file.write("RIFF", 4);
    file.write(reinterpret_cast<const char*>(&chunkSize), 4);
    file.write("WAVE", 4);
    file.write("fmt ", 4);
    int subchunk1Size = 16;
    int audioFormat = 1;
    file.write(reinterpret_cast<const char*>(&subchunk1Size), 4);
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&channels), 2);
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    file.write(reinterpret_cast<const char*>(&bitDepth), 2);
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&dataSize), 4);
}

} // namespace DSDConverter


int main(int argc, char* argv[]) {
    DSDConverter::DSFtoWAVConverter converter;
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <input.dsf> <output.wav> <bitwidth> <samplerate> <channels> [dop]\n";
        std::cerr << "\nDoP (DSD over PCM) Explanation:\n";
        std::cerr << " - DoP wraps DSD inside a PCM stream for compatibility with standard PCM transports (USB, SPDIF).\n";
        std::cerr << " - Only 24-bit PCM is officially supported for DoP, as it reserves 8 bits for DoP markers.\n";
        std::cerr << " - Use 'dop' flag only if your DAC supports DoP decoding.\n";
        std::cerr << "\nPossible Conversions:\n";
        for (const auto& config : DSDConverter::validConfigurations) {
            for (const auto& pair : config.second) {
                std::cerr << " - Sample Rate: " << config.first << " Hz, Channels: " << pair.first << ", Bit Width: " << pair.second << "-bit\n";
            }
        }
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    int bitWidth = std::stoi(argv[3]);
    int sampleRate = std::stoi(argv[4]);
    int channels = std::stoi(argv[5]);
    bool useDoP = (argc > 6) ? std::stoi(argv[6]) : false;
    
    if (converter.convert(inputFile, outputFile, bitWidth, sampleRate, channels, useDoP)) {
        std::cout << "Conversion successful!" << std::endl;
    }
    return 0;
}

#define _UNICODE
#define UNICODE
#include <winscard.h>
#pragma comment(lib, "winscard.lib")
#include <iostream>
#include <string>
#include <vector>
#include <tchar.h>
#include <iomanip>
#include <bitset>

int main() {
    SCARDCONTEXT context;

    LONG result = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &context);
    if(result != SCARD_S_SUCCESS) {
        std::cerr << "Failed to establish context\n";
        return 1;
    }

    SCARD_READERSTATE readerState = {};
    readerState.szReader = _T("HID OMNIKEY 5025-CL 0");
    readerState.dwCurrentState = SCARD_STATE_UNAWARE;

    while(!(readerState.dwEventState & SCARD_STATE_PRESENT)) {
        long ret = SCardGetStatusChange(context, 1, &readerState, 1);
    }

    SCARDHANDLE handle;
    DWORD activeProtocol;
    result = SCardConnect(context, _T("HID OMNIKEY 5025-CL 0"), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &handle, &activeProtocol);
    if(result != SCARD_S_SUCCESS) {
        std::cerr << "Failed to connect to card...\n";
        return 1;
    }

    SCARD_IO_REQUEST io_request;
    if(activeProtocol == SCARD_PROTOCOL_T0) {
        io_request = *SCARD_PCI_T0;
    }
    else {
        io_request = *SCARD_PCI_T1;
    }

    BYTE uid_packet[] = {0xFF, 0xCA, 0x00, 0x00, 0x00};
    BYTE vendor_packet[] = {0xFF, 0x70, 0x07, 0x6B, 0x00};
    DWORD packet_size = 5;
    BYTE response[32];
    DWORD response_size;

    result = SCardTransmit(handle, &io_request, uid_packet, packet_size, NULL, response, &response_size);
    if(result != SCARD_S_SUCCESS) {
        std::cerr << "Failed to transmit to card...\n";
        return 1;
    }

    std::string bitString = "";
    for(int i = 0; i < response_size; i++) {
        bitString = bitString + (std::bitset<8>(response[i])).to_string();
    }

    std::bitset<32> idBits = std::bitset<32>(bitString.substr(2,32));

    std::cout << std::hex << std::nouppercase << idBits.to_ulong();

}
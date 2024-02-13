#define _UNICODE
#define UNICODE
#include <winscard.h>
#pragma comment(lib, "winscard.lib")
#include <iostream>
#include <string>
#include <vector>
#include <tchar.h>

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

    BYTE packet[] = {0xFF, 0x70, 0x07, 0x6B, 0x00};
    DWORD packet_size = 5;
    BYTE response[64];
    DWORD response_size;

    result = SCardTransmit(handle, &io_request, packet, packet_size, NULL, response, &response_size);
    if(result != SCARD_S_SUCCESS) {
        std::cerr << "Failed to transmit to card...\n";
        return 1;
    }

    std::string response_string;
    for(int i = 0; i < response_size; i++) {
        response_string += std::to_string(response[i]);
        response_string += " ";
    }
    std::cout << response_string;

}
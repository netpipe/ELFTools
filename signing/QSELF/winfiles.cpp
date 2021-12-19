#ifdef Q_OS_WIN
int ReadFromExeFile() {
    BYTE buff[4096];
    DWORD read;
    BYTE* data;

    // Open exe file
    GetModuleFileNameA(NULL, (CHAR*)buff, sizeof(buff));
    //cout << buff << endl;

    HANDLE hFile = CreateFileA((CHAR*)buff, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
        return ERROR;

    ReadFile(hFile, buff, sizeof(buff), &read, NULL);
    IMAGE_DOS_HEADER* dosheader = (IMAGE_DOS_HEADER*)buff;

    // Locate PE header
    IMAGE_NT_HEADERS32* header = (IMAGE_NT_HEADERS32*)(buff + dosheader->e_lfanew);
    if (dosheader->e_magic != IMAGE_DOS_SIGNATURE || header->Signature != IMAGE_NT_SIGNATURE) {
        CloseHandle(hFile);
        return ERROR;
    }

    // For each section
    IMAGE_SECTION_HEADER* sectiontable = (IMAGE_SECTION_HEADER*)((BYTE*)header + sizeof(IMAGE_NT_HEADERS32));
    DWORD maxpointer = 0, exesize = 0;
    for (int i = 0; i < header->FileHeader.NumberOfSections; i++) {
        if (sectiontable->PointerToRawData > maxpointer) {
            maxpointer = sectiontable->PointerToRawData;
            exesize = sectiontable->PointerToRawData + sectiontable->SizeOfRawData;
        }
        sectiontable++;
    }

    // Seek to the overlay
    DWORD filesize = GetFileSize(hFile, NULL);
    SetFilePointer(hFile, exesize, NULL, FILE_BEGIN);
    data = (BYTE*)malloc(filesize - exesize + 1);
    ReadFile(hFile, data, filesize - exesize, &read, NULL);
    CloseHandle(hFile);

    //cout << filesize << " " << exesize << endl;

    // Process the data
    if (filesize != exesize) {
        *(data + (filesize - exesize)) = '\0';
        //cout << data << endl;

        HANDLE hFile1 = CreateFileA((CHAR*)"1.zip", GENERIC_WRITE, 0,
            NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE == hFile1)
            return ERROR;
        DWORD writtensize;
        WriteFile(hFile1, data, filesize - exesize + 1, &writtensize, NULL);
        CloseHandle(hFile1);
    }

    free(data);
    return 0;
}
#endif

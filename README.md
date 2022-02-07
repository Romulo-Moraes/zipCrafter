<div align="center">
    <h1>ZipCrafter</h1>
    <h3>C++ library to interact with zip files</h3>
</div>

# About
This is an C++ class based wrapper to interact with files, with this is possible read zip directories, read/write files, get file size, check if the file is encrypted, get number of entries, and do some previous operations by file index.

The wrapper has only an single-file, but need of libzip installed to work, i guess the libzip is hard to learn and the documentation is not so good, then i created this library.

# Documentation
### The file has an just one class (and some custom exceptions), then you just need instantiate it:

    // the var filename is of type string
    zipCrafter crafter(filename);

### then you can list all base files in the zip directory with the command (Is possible change the directory to read in the method args):

    vector<string> files = crafter.listDirectory();

### with the files name in memory we can do some reads:

    // path = path of file inside zip
    // size = size of content to read
    // source = char array pointer to assign the result
    crafter.readFile(path,size,source);

### if you first need knows the file size to execute an read you could use:

    // path = path of file inside zip
    int size = crafter.fileSize(path);

### the method readFile is forbbiden for directories, then you can check if this is an folder for each operation with:

    // path = path of file inside zip
    bool isFolder = crafter.checkIfItemIsFolder(path)

### the lib has resources to check if file/folder already exists inside the zip:

    // path = path of file inside zip
    bool exists = crafter.checkIfExists(path)


# Operation with index is available too

### you can start with this by the following command:

    int numberOfFiles = crafter.getEntriesNumber();


### then is possible read an position inside the range with (has possible exceptions):

    // path = path of file inside zip
    // index = position of file
    // size = size of content to read
    // source = char array pointer to assign the result

    readFileByIndex(index,size,source)

### check if is folder by index (has possible execeptions):

    bool isFolder = crafter.checkIfIsFolderByIndex(index);

### check file size by index (has possible execeptions):

    int size = crafter.fileSizeByIndex(index);

in the end of all operations you need to close the file:

    crafter.closeZip();


# Operations with encrypted files
### Some times the files inside zip can be encrypted, then you can with this lib check and try open with some password, yours or of the user.

<br/>

### Check if file is encrypted:

    bool result = crafter.checkIfEncrypted();

### Try open file with some password:

    //Return an bool to you know if was the correct password
    bool correctPassword = crafter.readEncryptedFile("main.cpp","romjijiulo",sizeToRead,buffer);

### Try open file with some password by index

    int sizeToRead = 10;
    char buffer[sizeToRead + 1];
    bool correctPassword = crafter.readEncryptedFileByIndex(0,"password",sizeToRead,buffer);



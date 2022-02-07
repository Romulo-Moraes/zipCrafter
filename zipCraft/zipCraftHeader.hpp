#include <zip.h>
#include <iostream>
#include <vector>
#include <filesystem>
#include <string.h>
#include <fstream>

#define AES_128_ENCRYPTION 0x0101
#define AES_192_ENCRYPTION 0x0102
#define AES_256_ENCRYPTION 0x0103

#define RAND_CHARS "sdokfjogfjerigjeigjergjtrigjtrigjtrugjtuhjuigjerigkeoepferoktphrphopghopgofpgotpogpfogpfgotphlrgplfdbplpdfgpeorepopeopeoeoo"

using namespace std;
namespace fs = std::filesystem;

class zipIsClosedException : exception
{
};
class zipAlreadyClosed : exception
{
};
class itemIsDirectoryExeception : exception
{
};
class itemDontExistsException : exception
{
};
class itemNotIsDirectory : exception
{
};
class readByIndexOutOfRange : exception
{
};
class fileSizeByIndexOutOfRange : exception
{
};
class replaceFileByIndexOutOfRange : exception
{
};
class cantCheckEncryptedNoFileInside : exception
{
};
class cantListDirTargetNotExists : exception
{
};
class readEncryptedFileByIndexOutOfRange : exception
{
};
class checkIfIsFolderByIndexOutOfRange : exception
{
};
class zipFileNotExists : exception
{
};
class theFileNotIsZip : exception
{
};
class zipFileCannotBeOpen : exception
{
};
class cantAllocateMemory : exception
{
};
class zipOpenDefaultError : exception
{
};
class unknowEncryptionMethodException : exception{};
class setPasswordToFileByIndexOutOfRange : exception{};

class zipCrafter
{
public:
    // Class constructor
    zipCrafter(string filename)
    {
        this->zipName = filename;
    };

    // Return the pointer of zip file to programmer
    //  The lib don't has any responsabilities about changes in the pointer
    zip *getLibzipFilePointer()
    {
        return this->z;
    }

    void openZip()
    {
        if (fs::exists(this->zipName))
        {
            this->executeZipOpen(false);
        }
        else
        {
            throw zipFileNotExists();
        }
    }

    void setPasswordToFile(string path,int encryption,string password){
        if(this->checkIfFileIsOpen()){
            if(checkIfExists(path)){
                if(checkIfEncryptionExists(encryption)){
                    struct zip_stat st;
                    zip_stat(this->z,path.c_str(),0,&st);
                    zip_file_set_encryption(this->z,st.index,encryption,password.c_str());
                }
                else{
                    throw unknowEncryptionMethodException();
                }
            }
            else{
                throw itemDontExistsException();
            }
        }   
        else{
            throw zipIsClosedException();
        }
    };

    void setPasswordToFileByIndex(int index,int encryption,string password){
        if(index < this->getEntriesNumber()){
            if(checkIfEncryptionExists(encryption)){
                zip_file_set_encryption(this->z,index,encryption,password.c_str());
            }
            else{
                throw unknowEncryptionMethodException();
            }
        }   
        else{
            throw setPasswordToFileByIndexOutOfRange();
        }
    };

    void createZip()
    {
        if (fs::exists(this->zipName))
        {
            fs::remove_all(this->zipName);
        }

        this->executeZipOpen(true);
    };

    // Get file size by index, error if out of range
    int fileSizeByIndex(int index)
    {
        if (checkIfFileIsOpen())
        {
            if (index < this->getEntriesNumber())
            {
                struct zip_stat st;
                zip_stat_index(this->z, index, 0, &st);
                return st.size;
            }
            else
            {
                throw fileSizeByIndexOutOfRange();
            }
        }
        else
        {
            throw zipIsClosedException();
        }
    };

    // Read file by index, error if out of range
    void readFileByIndex(unsigned int index, int size, char *source)
    {
        if (checkIfFileIsOpen())
        {
            if (true)
            {
                if (index < this->getEntriesNumber())
                {
                    zip_file *f = zip_fopen_index(this->z, index, 0);
                    char buffer[size + 1];
                    // Read file
                    zip_fread(f, &buffer, size);
                    buffer[sizeof(buffer) - 1] = '\0';
                    // Returning the buffer
                    strcpy(source, buffer);
                }
                else
                {
                    throw readByIndexOutOfRange();
                }
            }
            else
            {
                throw itemIsDirectoryExeception();
            }
        }
        else
        {
            throw zipIsClosedException();
        }
    };

    int getFileIndexByName(string filename){
        if(checkIfExists(filename)){
            struct zip_stat st;
            zip_stat(this->z,filename.c_str(),0,&st);
            return st.index;
        }
        else{
            throw itemDontExistsException();
        }
    };

    // Read an file
    void readFile(string path, int size, char *source)
    {
        if (checkIfFileIsOpen())
        {
            // struct zip_stat st;
            // Checking if exists
            if (checkIfExists(path))
            {
                // Making sure that isn't an directory
                if (!checkIfItemIsFolder(path))
                {
                    zip_file *f = zip_fopen(this->z, path.c_str(), 0);
                    char buffer[size + 1];
                    // Read file
                    zip_fread(f, &buffer, size);
                    buffer[sizeof(buffer) - 1] = '\0';
                    // Returning the buffer
                    strcpy(source, buffer);
                }
                else
                {
                    throw itemIsDirectoryExeception();
                }
            }
            else
            {
                throw itemDontExistsException();
            }
        }
        else
        {
            throw zipIsClosedException();
        }
    }

    // Method to write file inside zip file
    void writeFile(string path, const char buffer[],int sizeToWrite)
    {
        if (checkIfFileIsOpen())
        {
            zip_source_t *source;
            source = zip_source_buffer(this->z, buffer, sizeToWrite, 0);
            zip_file_add(this->z, path.c_str(), source,0);
            this->writeFileInDisk();
        }
        else
        {
            throw zipIsClosedException();
        }
    };

    bool checkIfIsFolderByIndex(unsigned int index)
    {
        if (index < this->getEntriesNumber())
        {
            struct zip_stat st;
            zip_stat_index(this->z, index, 0, &st);
            if (st.name[strlen(st.name) - 1] == '/')
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            throw checkIfIsFolderByIndexOutOfRange();
        }
    };

    // Replace an file by index, error if out of range
    int replaceFileByIndex(int index, char buffer[],int sizeToWrite)
    {
        if (checkIfFileIsOpen())
        {
            if (index < this->getEntriesNumber())
            {
                struct zip_stat st;
                zip_stat_index(this->z, index, 0, &st);
                this->writeFile(st.name, buffer,sizeToWrite);
                this->writeFileInDisk();
            }
            else
            {
                throw replaceFileByIndexOutOfRange();
            }
        }
        else
        {
            throw zipIsClosedException();
        }
    };

    // Close the entire file
    void closeZip()
    {
        if (this->zipIsOpen)
        {
            zip_close(this->z);
        }
        else
        {
            throw zipAlreadyClosed();
        }
    }

    // Get number of files/folders in the zip
    int getEntriesNumber()
    {
        if (checkIfFileIsOpen())
        {
            zip_uint64_t entries = zip_get_num_entries(this->z, 0);
            return entries;
        }
        else
        {
            throw zipIsClosedException();
        }
    };

    bool readEncryptedFileByIndex(int index, string password, int size, char source[])
    {
        if (index < this->getEntriesNumber())
        {
            if (!checkIfIsFolderByIndex(index))
            {
                zip_file *file = zip_fopen_index_encrypted(this->z, index, 0, password.c_str());
                if (file == NULL)
                {
                    return false;
                }
                else
                {
                    char buffer[size];
                    zip_fread(file, buffer, size);
                    strcpy(source, buffer);
                    return true;
                }
            }
            else
            {
                throw itemIsDirectoryExeception();
            }
        }
        else
        {
            throw readEncryptedFileByIndexOutOfRange();
        }
    };

    bool readEncryptedFile(string path, string password, int size, char source[])
    {
        if (this->checkIfExists(path))
        {
            if (!this->checkIfItemIsFolder(path))
            {
                zip_file *file = zip_fopen_encrypted(this->z, path.c_str(), 0, password.c_str());
                if (file != NULL)
                {
                    char buffer[size];
                    zip_fread(file, buffer, size);
                    buffer[sizeof(buffer) - 1] = '\0';
                    strcpy(source, buffer);
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                throw itemIsDirectoryExeception();
            }
        }
        else
        {
            throw itemDontExistsException();
        }
    };

    zip_uint64_t fileSize(string path)
    {
        if (checkIfFileIsOpen())
        {
            if (checkIfExists(path))
            {
                if (!checkIfItemIsFolder(path))
                {
                    struct zip_stat st;
                    zip_stat(this->z, path.c_str(), 0, &st);
                    return st.size;
                }
                else
                {
                    throw itemIsDirectoryExeception();
                }
            }
            else
            {
                cout << "not exists";
                return 0;
            }
        }
        else
        {
            throw zipIsClosedException();
        }
    };

    bool checkIfEncrypted()
    {
        if (this->getEntriesNumber() > 0)
        {
            zip_file *file = zip_fopen_index_encrypted(this->z, 0, 0, RAND_CHARS);
            zip_file *file2 = zip_fopen_index_encrypted(this->z, 0, 0, "");
            bool fileNull = (file == NULL);
            bool file2Null = (file2 == NULL);
            if (fileNull == true || file2Null == true)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            throw cantCheckEncryptedNoFileInside();
        }
    }

    // List the  base directory by default, but is possible specify the target
    vector<string> listDirectory(string directory = "")
    {
        vector<string> allFiles;
        if (checkIfFileIsOpen())
        {
            if (directory == "" || checkIfExists(directory))
            {
                zip_uint64_t entries = zip_get_num_entries(this->z, 0);
                int sizeOfSpecifiedDirectory = directory.length();
                string stringToCheckDirectoryEquals;
                string itemOfDirectoryEquals;

                // If the dir is default
                if (directory == "")
                {
                    for (zip_uint64_t i = 0; i < entries; i++)
                    {
                        allFiles.push_back(zip_get_name(z, i, 0));
                    }
                }
                else
                {
                    // Checking if the item is folder, only folders can be listed
                    if (checkIfItemIsFolder(directory))
                    {
                        // Loop to get each item in zip file
                        for (zip_int64_t i = 0; i < entries; i++)
                        {
                            // Get name
                            stringToCheckDirectoryEquals = zip_get_name(this->z, i, 0);
                            // Substring to check if the file is enside the specified directory
                            stringToCheckDirectoryEquals = stringToCheckDirectoryEquals.substr(0, sizeOfSpecifiedDirectory);
                            // Checking
                            if (stringToCheckDirectoryEquals == directory)
                            {
                                // Get the name again and remove the initial values, to only return the file/folder name
                                itemOfDirectoryEquals = zip_get_name(this->z, i, 0);
                                itemOfDirectoryEquals = itemOfDirectoryEquals.substr(sizeOfSpecifiedDirectory);
                                // If the result is not equal to empty or folder results, then...
                                if (itemOfDirectoryEquals != "" && itemOfDirectoryEquals != "/")
                                {
                                    // Remove any '/' prefix from the file;
                                    if (itemOfDirectoryEquals[0] == '/')
                                    {
                                        allFiles.push_back(itemOfDirectoryEquals.substr(1));
                                    }
                                    else
                                    {
                                        allFiles.push_back(itemOfDirectoryEquals);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        // Is not a folder, then assign error code to pointer;
                        throw itemNotIsDirectory();
                    }
                }
                return allFiles;
            }
            else
            {
                throw cantListDirTargetNotExists();
            }
        }
        else
        {
            throw zipIsClosedException();
        }
    }

    bool checkIfItemIsFolder(string directory)
    {
        if (directory[directory.length() - 1] != '/')
        {
            directory += '/';
        }
        if (checkIfExists(directory))
        {
            return true;
        }
        return false;
    }

    bool checkIfExists(string filename)
    {
        if (checkIfFileIsOpen())
        {
            struct zip_stat st;
            zip_stat(this->z, filename.c_str(), 0, &st);
            if (st.valid == 255)
            {
                return true;
            }
            return false;
        }
        else
        {
            throw zipIsClosedException();
        }
    };

    bool checkIfFileIsOpen()
    {
        return this->zipIsOpen;
    }

private:
    zip *z;
    int *errorCode = 0;
    int openZipError;
    bool zipIsOpen = false;
    string zipName;

    void writeFileInDisk(){
        this->closeZip();
        this->openZip();
    };

    void handleOpenZipErrors()
    {
        if (this->errorCode != 0)
        {
            switch (*this->errorCode)
            {
            case ZIP_ER_NOZIP:
                throw theFileNotIsZip();
                break;
            case ZIP_ER_OPEN:
                throw zipFileCannotBeOpen();
                break;
            case ZIP_ER_MEMORY:
                throw cantAllocateMemory();
                break;
            default:
                throw zipOpenDefaultError();
                break;
            }
        }
    };

    bool checkIfEncryptionExists(int encryption){
        if(encryption == AES_128_ENCRYPTION || encryption == AES_192_ENCRYPTION || encryption == AES_256_ENCRYPTION){
            return true;
        }
        return false;
    }

    void executeZipOpen(bool isCreate)
    {
        if(isCreate){
            // Try open the file with specified name
            this->z = zip_open(this->zipName.c_str(), ZIP_CREATE, this->errorCode);
        }
        else{
            // Try open the file with specified name
            this->z = zip_open(this->zipName.c_str(), 0, this->errorCode);
        }
        if (this->errorCode == 0)
        {
            this->zipIsOpen = true;
        }
    }
};
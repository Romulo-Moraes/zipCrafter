#include <zip.h>
#include <iostream>
#include <vector>
#include <string.h>

#define ITEM_ISNT_DIR 2001
#define ITEM_IS_DIR 2002
#define ITEM_NOT_EXISTS 2003
#define ZIP_IS_CLOSED 2004
#define RAND_CHARS "sdokfjogfjerigjeigjergjtrigjtrigjtrugjtuhjuigjerigkeoepferoktphrphopghopgofpgotpogpfogpfgotphlrgplfdbplpdfgpeorepopeopeoeoo"

using namespace std;

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
class readEncryptedFileByIndexOutOfRange : exception{
};
class checkIfIsFolderByIndexOutOfRange : exception{
};

class zipCrafter
{
public:
    // Class constructor
    zipCrafter(string filename)
    {
        // Try open the file with specified name
        this->z = zip_open(filename.c_str(), 0, this->errorCode);
        if (this->errorCode == 0)
        {
            this->zipIsOpen = true;
        }
        // Handle some error if has anyone
        // this->handleOpenErrors();
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
            // struct zip_stat st;
            // Checking if exists
            // Making sure that isn't an directory
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
    void writeFile(string path, char buffer[])
    {
        if (checkIfFileIsOpen())
        {
            zip_source_t *source;
            source = zip_source_buffer(this->z, buffer, 10, 0);
            zip_file_add(this->z, path.c_str(), source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8);
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
    int replaceFileByIndex(int index, char buffer[])
    {
        if (checkIfFileIsOpen())
        {
            if (index < this->getEntriesNumber())
            {
                struct zip_stat st;
                zip_stat_index(this->z, index, 0, &st);
                this->writeFile(st.name, buffer);
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

    bool readEncryptedFileByIndex(int index,string password,int size,char source[]){
        if(index < this->getEntriesNumber()){
            if(!checkIfIsFolderByIndex(index)){
                zip_file *file = zip_fopen_index_encrypted(this->z,index,0,password.c_str());
                if(file == NULL){
                    return false;
                }
                else{
                    char buffer[size];
                    zip_fread(file,buffer,size);
                    strcpy(source,buffer);
                    return true;
                }
            }
            else{
                throw itemIsDirectoryExeception();
            }
        }
        else{
            throw readEncryptedFileByIndexOutOfRange();
        }
    };

    bool readEncryptedFile(string path,string password,int size,char source[]){
        if(this->checkIfExists(path)){
            if(!this->checkIfItemIsFolder(path)){
                zip_file *file = zip_fopen_encrypted(this->z,path.c_str(),0,password.c_str());
                if(file != NULL){
                    char buffer[size];
                    zip_fread(file,buffer,size);
                    buffer[sizeof(buffer) -1] = '\0';
                    strcpy(source,buffer);
                    return true;
                }
                else{
                    return false;
                }
            }   
            else{
                throw itemIsDirectoryExeception();
            }
        }
        else{
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
            if(fileNull == true || file2Null == true)
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

    // Get any error meaning from zip/zipCraft lib
    string getErrorMeaning(int code)
    {
        // Less than 2000 is an error of zip lib
        if (code < 2000)
        {
            zip_error_t error;
            zip_error_init_with_code(&error, code);

            return zip_error_strerror(&error);
        }
        // Greater is an zipCraft error
        else
        {
            switch (code)
            {
            case ITEM_ISNT_DIR:
                return "The item not is a directory";

            default:
                return "Unknow error";
            }
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

private:
    zip *z;
    int *errorCode = 0;
    int openZipError;
    bool zipIsOpen = false;

    bool checkIfFileIsOpen()
    {
        return this->zipIsOpen;
    }
};
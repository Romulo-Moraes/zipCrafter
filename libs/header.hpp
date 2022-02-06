#include <zip.h>
#include <iostream>
#include <vector>

#define ITEM_ISNT_DIR 2001
#define ITEM_IS_DIR 2002
#define ITEM_NOT_EXISTS 2003
#define ZIP_IS_CLOSED 2004

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

class zipItem
{
public:
    string name;
    int size;
    bool isDirectory;
    bool exists;
    zip_file *file;

    zipItem(string name, int size, bool isDirectory, bool exists, bool *motherClassIsOpen, zip *motherClassZ, zip_file *file)
    {
        this->name = name;
        this->size = size;
        this->isDirectory = isDirectory;
        this->exists = exists;
        this->motherClassIsOpen = motherClassIsOpen;
        this->motherClassZ = motherClassZ;
        this->file = file;
    }

    string readFile(int sizeToRead)
    {
        if (checkIfFileIsOpen())
        {
            struct zip_stat st;
            // Checking if exists
            // Making sure that isn't an directory
            char buffer[sizeToRead + 1];
            // Read file
            zip_fread(this->file, &buffer, sizeToRead);
            buffer[sizeof(buffer) - 1] = '\0';
            // Returning the buffer
            return buffer;
        }
        else
        {
            throw zipIsClosedException();
        }
    }

    void writeFile(const char buffer[]){
        if(checkIfFileIsOpen()){
            zip_source_t* zs;

            zs = zip_source_buffer(this->motherClassZ, "aaaaa", 5, 0);

            zip_file_add(this->motherClassZ, "tesssst.txt", zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8);

            zip_close(this->motherClassZ);
            // zip_source_t *source;
            // cout << "alo";
            // source = zip_source_buffer(this->motherClassZ,"sdiofjsdifjsdfijsdijf",14,0);
            // zip_file_add(this->motherClassZ,this->name.c_str(),source,ZIP_FL_OVERWRITE);
            // zip_close(this->motherClassZ);
            // *this->motherClassIsOpen = false;
        }  
        else{
            throw zipIsClosedException();
        }
    };

    // Check if some item exists in the zip file
    bool checkIfExists(string filename)
    {
        if (checkIfFileIsOpen())
        {
            struct zip_stat st;
            zip_stat(this->motherClassZ, filename.c_str(), 0, &st);
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

    // Method to check if item is folder looking for '/' in the end of name
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

private:
    bool *motherClassIsOpen;
    zip *motherClassZ;

    bool checkIfFileIsOpen()
    {
        return *motherClassIsOpen;
    }
};

class zipCrafter
{
public:
    // Class constructor
    zipCrafter(string filename)
    {
        // Try open the file with specified name
        this->z = zip_open(filename.c_str(), ZIP_CREATE, this->errorCode);
        if (this->errorCode == 0)
        {
            this->zipIsOpen = true;
        }
        // Handle some error if has anyone
        // this->handleOpenErrors();
    };

    // Method to write file inside zip file
    void writeFile(string path, const char content[])
    {
        if (checkIfFileIsOpen())
        {
            zip_source_t *source;
            source = zip_source_buffer(this->z, content, sizeof(*content), 0);
            zip_file_add(this->z, path.c_str(), source, 0);
        }
        else
        {
            throw zipIsClosedException();
        }
    };

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
    zipItem getItem(string path)
    {
        struct zip_stat st;
        bool isDirectory = false;
        zip_stat(this->z, path.c_str(), 0, &st);

        if (st.valid)
        {

            if (st.name[sizeof(st.name) - 1] == '/')
            {
                isDirectory = true;
            }

            zip_file *f = zip_fopen(this->z, path.c_str(), 0);

            return zipItem(st.name, st.size, isDirectory, true, &this->zipIsOpen, this->z, f);
        }
        else
        {
            return zipItem("", 0, false, false, NULL, NULL, NULL);
        }
    };
    // Method to read file

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

    // List the  base directory by default, but is possible specify the target
    vector<string> listDirectory(string directory = "")
    {
        vector<string> allFiles;
        if (checkIfFileIsOpen())
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

    // The method to handle any zip open error
    // void handleOpenErrors(){
    //     if(this->openZipError != 0){
    //         zip_error_t error;
    //         zip_error_init_with_code(&error,*this->errorCode);

    //         *this->errorCode = 1;
    //     }else{

    //     }
    // };
};
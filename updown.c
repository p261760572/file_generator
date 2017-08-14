#include <stdlib.h>  
#include <stdio.h>  
#include <curl/curl.h>  
#include <sys/stat.h>  
size_t getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream)   
{  
    int r;  
    long len = 0;  
    r = sscanf((const char*)ptr, "Content-Length: %ld\n", &len);  
    if (r) 
        *((long *) stream) = len;  
    return size * nmemb;  
}  
/* discard downloaded data */  
size_t discardfunc(void *ptr, size_t size, size_t nmemb, void *stream)   
{  
    return size * nmemb;  
}  
//write data to upload  
size_t writefunc(void *ptr, size_t size, size_t nmemb, void *stream)  
{  
    return fwrite(ptr, size, nmemb, (FILE*)stream);  
}  
/* read data to upload */  
size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream)  
{  
    FILE *f = (FILE*)stream;  
    size_t n;  
    if (ferror(f))  
        return CURL_READFUNC_ABORT;  
    n = fread(ptr, size, nmemb, f) * size;  
    return n;  
}  
int upload(const char * remotepath, const char * localpath, const char *userpass, long timeout, long tries)  
{  
    FILE *f;  
    long uploaded_len = 0;  
    CURLcode r = CURLE_GOT_NOTHING;  
    int c;  
    f = fopen(localpath, "rb");  
    if (f == NULL) {  
        perror(NULL);  
        return 0;  
    }  
    CURL *curlhandle = NULL;  
    CURL *curldwn = NULL;  
    curl_global_init(CURL_GLOBAL_ALL);  
    curlhandle = curl_easy_init();  
    curldwn = curl_easy_init();  
    curl_easy_setopt(curlhandle, CURLOPT_UPLOAD, 1L);  
    curl_easy_setopt(curlhandle, CURLOPT_URL, remotepath);  
    curl_easy_setopt(curlhandle, CURLOPT_USERPWD, userpass);     
    if (timeout)  
        curl_easy_setopt(curlhandle, CURLOPT_FTP_RESPONSE_TIMEOUT, timeout);  
    curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);  
    curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &uploaded_len);  
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, discardfunc);  
    curl_easy_setopt(curlhandle, CURLOPT_READFUNCTION, readfunc);  
    curl_easy_setopt(curlhandle, CURLOPT_READDATA, f);  
    curl_easy_setopt(curlhandle, CURLOPT_FTPPORT, "-"); /* disable passive mode */  
//   curl_easy_setopt(curlhandle, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);  
    curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);  
    for (c = 0; (r != CURLE_OK) && (c < tries); c++) {  
        /* are we resuming? */  
        if (c) { /* yes */  
            /* determine the length of the file already written */  
            /* 
            * With NOBODY and NOHEADER, libcurl will issue a SIZE 
            * command, but the only way to retrieve the result is 
            * to parse the returned Content-Length header. Thus, 
            * getcontentlengthfunc(). We need discardfunc() above 
            * because HEADER will dump the headers to stdout 
            * without it. 
            */  
            curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 1L);  
            curl_easy_setopt(curlhandle, CURLOPT_HEADER, 1L);  
            r = curl_easy_perform(curlhandle);  
            if (r != CURLE_OK)  
                continue;  
            curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 0L);  
            curl_easy_setopt(curlhandle, CURLOPT_HEADER, 0L);  
            fseek(f, uploaded_len, SEEK_SET);  
            curl_easy_setopt(curlhandle, CURLOPT_APPEND, 1L);  
        }  
        else { /* no */  
            curl_easy_setopt(curlhandle, CURLOPT_APPEND, 0L);  
        }  
        r = curl_easy_perform(curlhandle);  
    }  
    fclose(f);  
    curl_easy_cleanup(curlhandle);  
    curl_easy_cleanup(curldwn);  
    curl_global_cleanup();  
    if (r == CURLE_OK)  
        return 1;  
    else {  
        return -1;  
    }  
}  
// ����  
int download(const char * remotepath, const char * localpath, const char *userpass, long timeout, long tries)  
{  
    FILE *f;  
    curl_off_t local_file_len = -1 ;  
    long filesize =0 ;  
    CURLcode r = CURLE_GOT_NOTHING;  
    struct stat file_info;  
    int use_resume = 0;  
    //��ȡ�����ļ���С��Ϣ  
    if(stat(localpath, &file_info) == 0)  
    {  
        local_file_len = file_info.st_size;   
        use_resume = 1;  
    }  
    //׷�ӷ�ʽ���ļ���ʵ�ֶϵ�����  
    f = fopen(localpath, "ab+");  
    if (f == NULL) {  
        perror(NULL);  
        return 0;  
    }  
    CURL *curlhandle = NULL;  
    CURL *curldwn = NULL;  
    curl_global_init(CURL_GLOBAL_ALL);  
    curlhandle = curl_easy_init();  
    curldwn = curl_easy_init();  
    curl_easy_setopt(curlhandle, CURLOPT_URL, remotepath);  
    curl_easy_setopt(curlhandle, CURLOPT_USERPWD, userpass);     
    //���ӳ�ʱ����  
    curl_easy_setopt(curlhandle, CURLOPT_CONNECTTIMEOUT, timeout);  
    //����ͷ������  
    curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);  
    curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &filesize);  
    // ���öϵ�����  
    curl_easy_setopt(curlhandle, CURLOPT_RESUME_FROM_LARGE, use_resume?local_file_len:0);  
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, writefunc);  
    curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, f);  
    curl_easy_setopt(curlhandle, CURLOPT_NOPROGRESS, 1L);  
    curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);  
    r = curl_easy_perform(curlhandle);  
    fclose(f);  
    curl_easy_cleanup(curlhandle);  
    curl_easy_cleanup(curldwn);  
    curl_global_cleanup();  
    if (r == CURLE_OK)  
        return 1;  
    else {  
        return -1;  
    }  
}  

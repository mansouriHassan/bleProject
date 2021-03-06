#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include <curl/curl.h>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/ostreamwrapper.h"

namespace
{
    std::size_t callback(
            const char* in,
            std::size_t size,
            std::size_t num,
            std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }
}

using namespace rapidjson;

int main()
{
    const std::string url("http://date.jsontest.com/");

    CURL* curl = curl_easy_init();

    // Set remote URL.
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Don't bother trying IPv6, which would increase DNS resolution time.
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    // Don't wait forever, time out after 10 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Response information.
    long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (httpCode == 200)
    {
        //std::cout << "\nGot successful response from " << url << std::endl;

        std::string data = *(httpData.get());
        std::cout << "data = " << data << std::endl;

        const char* json = data.c_str();
        Document document;
        document.Parse(json);

        printf("\nAccess values in document:\n");
        assert(document.IsObject());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.

        assert(document.HasMember("date"));
        assert(document["date"].IsString());
        printf("date = %s\n", document["date"].GetString());

        assert(document.HasMember("milliseconds_since_epoch"));
        assert(document["milliseconds_since_epoch"].IsInt64());
        printf("milliseconds_since_epoch = %ld\n", document["milliseconds_since_epoch"].GetInt64());

        assert(document.HasMember("time"));
        assert(document["time"].IsString());
        printf("time = %s\n", document["time"].GetString());


    }
    else
    {
        std::cout << "Couldn't GET from " << url << " - exiting" << std::endl;
        return 1;
    }

    return 0;
}
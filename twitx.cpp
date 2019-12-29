#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>
#include "json.hpp"
#include <regex>
#include <ctime>

using json = nlohmann::json;

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

struct http_get{
    std::string lon, lat, date;
    std::string geocode;
    json tweet;
    std::string StartTweet = "<p class=\"TweetTextSize  js-tweet-text tweet-text\" lang=\"in\" data-aria-label-part=\"0\">";
    std::string EndTweet = "</p>";
    std::string StartUser = "<span class=\"username u-dir u-textTruncate\" dir=\"ltr\" data-aria-label-part>@<b>";
    std::string EndUser = "</b>";
    std::string rTweet = "("+StartTweet+")(.*\n?)("+EndTweet+")";
    std::string rUsername = "("+StartUser+")(.*\n?)("+EndUser+")";

    void get(std::string);
    void scraperUser(std::string, std::string); 
    void scraperTweet(std::string, std::string);
    void dateTweet();
    std::string getData(std::string Sstart, std::string Send, std::string data);

    std::vector<std::string> usertweet, username;
} http;

int main(int argc, char** argv) {
    http.dateTweet();
    std::cout<<"Latitude: ";
    std::cin>>http.lat;
    std::cout<<"Longtitude: ";
    std::cin>>http.lon;
    http.geocode = http.lat+"%2C"+http.lon;
    std::string tweetUrl = "https://twitter.com/i/search/timeline?vertical=default&q=geocode%3A"+http.geocode+"%2C1km%20%20until%3A2019-12-27%20since%3A2019-12-26&src=typd&composed_count=0&include_available_features=1&include_entities=1&include_new_items_bar=true&interval=30000&latent_count=1";
    // std::cout<<tweetUrl;exit(1);
    http.get(tweetUrl);
    std::string tweett = http.tweet["items_html"];
    // std::cout<<tweett;exit(1);
    http.scraperUser(tweett, http.rUsername);
    http.scraperTweet(tweett, http.rTweet);
    for (int i = 0;i < 5; i++){
        std::cout<<"┏━@"<<http.username[i]<<std::endl;
        std::cout<<"┗━> ";
        try{
            std::cout<<http.usertweet[i]<<std::endl;
        } catch(const std::exception& e){
            std::cout<<std::endl;
            continue;
        }        
    }
    return 0;
}

void http_get::dateTweet(){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,sizeof(buffer),"%Y-%m-%d-1",timeinfo);
    std::string str(buffer);
    this->date;
}

void http_get::get(std::string u){
    auto curl = curl_easy_init();
    const char *url = u.c_str();
    json j;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        std::string response_string;
        std::string header_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
        char* url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl = NULL;
        this->tweet = json::parse(response_string);
    }
}
void http_get::scraperUser(std::string html, std::string regex){
    std::string s = html;
    std::regex r(regex);
    std::smatch m;

    while (std::regex_search(s,m,r)){
        // std::cout<<m[0];break;
        http.username.push_back(http_get::getData(http.StartUser, http.EndUser,m[0]));
        s = m.suffix();
    }    
}
void http_get::scraperTweet(std::string html, std::string regex){
    std::string s = html;
    std::regex r(regex);
    std::smatch m;

    while (std::regex_search(s,m,r)){
        http.usertweet.push_back(http_get::getData(http.StartTweet, http.EndTweet,m[0]));
        s = m.suffix();
    }    
}
std::string http_get::getData(std::string Sstart, std::string Send, std::string data){
    std::string str = data;
    std::string get = Sstart;
    std::string last = Send;
    std::string li, str1;
    bool isWrite = true;
    for (int x=0; x<=(str.length()-1); x++){
        isWrite=true;
        if (get.compare(li)==0){
            while (isWrite){
                if ((str[x]=='<' && str[x+1]=='/') || (str[x]=='<' && str[x+1]=='i') || (str[x]=='<' && str[x+1]=='a')){
                    isWrite=false;
                    li="";
                } else{
                    str1+=str[x];
                 }
                li+=str[x];
                x++;
            }
        } else if (str[x]==' ' && str[x+1]=='<'){
            li+=str[x];
        }
        li+=str[x];
    }
    return str1;
}
    // https://twitter.com/search?q=geocode:-8.6500,115.2167,1km  until:2019-12-18 since:2019-12-17&src=typed_query
    // https://twitter.com/i/search/timeline?vertical=default&q=geocode%3A-8.6500%2C115.2167%2C1km%20%20until%3A2019-12-18%20since%3A2019-12-17&src=typd&composed_count=0&include_available_features=1&include_entities=1&include_new_items_bar=true&interval=30000&latent_count=1std::cout<<"STOP"<<std::endl;
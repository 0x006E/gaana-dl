#include <iostream>
#include "fmt/include/fmt/format.h"
#include <cpr/cpr.h>
#include "md5/md5.h"
#include <string>

void eraseSubStr(std::string & mainStr, const std::string & toErase)
{
	// Search for the substring in string
	size_t pos = mainStr.find(toErase);
 
	if (pos != std::string::npos)
	{
		// If found then erase it from string
		mainStr.erase(pos, toErase.length());
	}
}

std::string generateHashToken(std::string PHPSESSID) {
int trackid = 1322901;
//PHPSESSID = "tgmpq9lc5n53akidd2aee42qk4";
std::string addedAll = std::to_string(trackid)+"|"+PHPSESSID+"|03:40:31 sec"; //std::to_string(trackid)+"|"+PHPSESSID+"|3:40:31 sec";
const char* BYTES = addedAll.c_str(); 
md5::md5_t md5;
md5.process(BYTES, strlen(BYTES));
md5.finish();
char str[MD5_STRING_SIZE];
md5.get_string(str);
std::string hashToken;
for (unsigned int i = 0; i < MD5_STRING_SIZE; i++)
	hashToken+=str[i];
return (hashToken + PHPSESSID.substr(3,6) + "=");
}

int main(int argc, char** argv) {
    cpr::Session session;
    //session.SetProxies({{"http", "http://127.0.0.1:8080"}});
    session.SetUrl(cpr::Url{"https://www.gaana.com"});
    session.SetVerifySsl(cpr::VerifySsl{false});
    auto firstResp = session.Get();
    std::cout << "Sending Request" << std::endl;
    std::string PHPSESSID = firstResp.cookies["PHPSESSID"];
    std::cout << fmt::format("Got PHP Session ID : {0}",PHPSESSID) << std::endl;
    std::cout << "Generating HashToken" << std::endl;
    std::string hashToken = generateHashToken(PHPSESSID);
    std::cout << fmt::format("Got HashToken : {0}",hashToken) << std::endl;
    std::cout << "Sending Request" << std::endl;
    session.SetUrl(cpr::Url{"https://apiv2.gaana.com/track/stream"});
    auto Payload = cpr::Payload{{"ht", hashToken},{"request_type","web"},{"track_id","1322901"},{"quality","high"},{"st","hls"},{"ssl","true"},{"c","a"}};
    eraseSubStr(Payload.content,"%00");
    session.SetPayload(Payload);
    auto secndResp = session.Post();
    std::cout << fmt::format("Got Content : {0}",secndResp.text);
    return 0;
}

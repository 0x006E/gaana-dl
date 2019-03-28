#include <iostream>
#include <fmt/format.h>
#include <cpr/cpr.h>
#include <md5.h>
#include <rapidjson/document.h>
#include <fstream>
#include <clipp.h>
#include <thread>
#include <chrono>

class progress {
public:
    void update(int num,int outOf, char s) 
    {
      std::string progString = "\rProgress: [";
      int numChar = (num*cols)/outOf;
      for(int loops=0;loops<numChar;loops++ )
        progString+=s;
      for(int loops=1;loops<(cols-numChar);loops++)
        progString+=" ";
      int percent = (num*100)/outOf;
      std::cout << progString << "](" << percent << "%)" << std::flush;
    }
private:
    int cols=60;
};

std::vector<std::string> tokenize(std::string sentence)
{
  std::stringstream ss(sentence);
  std::string to;
  std::vector<std::string> tokens;
  if (sentence != "")
  {
    while(std::getline(ss,to,'\n')){
      tokens.push_back(to);
    }
  }

return tokens;
}

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

std::string generateHashToken(int trackid, std::string PHPSESSID) {
std::string addedAll = fmt::format("{0}|{1}|03:40:31 sec",trackid,PHPSESSID);
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

//std::vector<std::string> scapreMetadata(std::string response)
void scapreMetadata(std::string response)
{

}

int main(int argc, char** argv) {
    std::string gaanaURL = "";
    auto cli = (
        clipp::required("-u", "--url") & clipp::value("URL for the gaana song", gaanaURL)
    );
    if(!parse(argc, argv, cli)) {std::cout << make_man_page(cli, argv[0]);return(0);}
    std::vector<std::string> streamUrl;
    cpr::Session session;
    cpr::Url URL;
    URL = gaanaURL;
    //session.SetProxies({{"http", "http://127.0.0.1:8080"}});
    session.SetUrl(URL);
    session.SetVerifySsl(cpr::VerifySsl{false});
    std::cout << "Sending Request" << std::endl;
    auto Response = session.Get();
    scapreMetadata(Response.text);
    std::string PHPSESSID = Response.cookies["PHPSESSID"];
    std::cout << fmt::format("Got PHP Session ID : {0}",PHPSESSID) << std::endl;
    std::cout << "Generating HashToken" << std::endl;
    int trackidArg=243233;
    std::string hashToken = generateHashToken(trackidArg, PHPSESSID);
    std::cout << fmt::format("Got HashToken : {0}",hashToken) << std::endl;
    std::cout << "Sending Request" << std::endl;
    URL = "https://apiv2.gaana.com/track/stream";
    session.SetUrl(URL);
    auto Payload = cpr::Payload{{"ht", hashToken},{"request_type","web"},{"track_id",std::to_string(trackidArg)},{"quality","high"},{"st","hls"},{"ssl","true"},{"c","a"}};
    eraseSubStr(Payload.content,"%00");
    session.SetPayload(Payload);
    Response = session.Post();
    std::cout << fmt::format("Got Content : \n{0}\n",Response.text);
    std::cout << "Parsing the Response" << std::endl;
    rapidjson::Document resp;
    resp.Parse(Response.text.c_str());
    std::cout << fmt::format("Got Stream Path : {0}",resp["stream_path"].GetString()) << std::endl;
    URL = resp["stream_path"].GetString();
    session.SetUrl(URL);
    std::cout << "Sending Request" << std::endl;
    session.SetPayload(cpr::Payload{});
    Response = session.Get();
    std::cout << fmt::format("Got Response : \n{0}",Response.text) << std::endl;
    std::vector<std::string> tokens = tokenize(Response.text);
    std::cout << fmt::format("Got Index : \n{0}\n",tokens[2]);
    URL = tokens[2];
    session.SetUrl(URL);
    Response = session.Get();
    std::cout << fmt::format("Got Response : \n{0}",Response.text) << std::endl;
    tokens = tokenize(Response.text);
    std::ofstream ts;
    progress downBar;
    for(int i=0; i<tokens.size(); i++)
    {
        if(tokens[i][0]=='#')
            continue;
        streamUrl.push_back(tokens[i]);
    }
    ts.open(fmt::format("/home/ntsv/songs/{0}.ts",trackidArg));
    for(int i=1; i<=streamUrl.size(); i++){
        URL = streamUrl[i];
        session.SetUrl(URL);
        auto stream = session.Get();
        ts << stream.text;
        downBar.update(i,streamUrl.size(),'#');
    }
    ts.close();
    std::cout << std::endl;
    return 0;
}

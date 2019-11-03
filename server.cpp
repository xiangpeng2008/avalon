#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <math.h>
#include <algorithm> 
#include <fstream>
#include <sstream>
#include <random> 
#include <typeinfo>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <netinet/in.h>

typedef int64_t ll;
#define loop(n) for(int64_t i=0;i<n;++i)
#define loop1(n) for(int64_t i=1;i<=n;++i)
#define loopab(a,b) for(int64_t i=a;i<b;++i)
#define max(a,b) a>b?a:b
#define cout(x) std::cout<<x<<endl;
#define printd(x) printf("%.10f\n",mm);

template <typename Out>
void split(const std::string &s, char delim, Out result) {
  std::istringstream iss(s);
  std::string item;
  while (std::getline(iss, item, delim)) {
    *result++ = item;
  }
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, std::back_inserter(elems));
  return elems;
}

class Role{
  public:
    Role(const std::string & name,const bool & good,const std::string & discription):name(name), good(good),discription(discription){}
    void setSkills(std::vector<Role*> roles){skill=roles;}
    const std::string name;
    const std::string discription;
    const bool good;
    std::vector<Role*> skill;
};

std::ostream &operator<<(std::ostream &os, Role const &role) { return os << role.name; }

struct RoleCompare { bool operator() (const Role& lhs, const Role& rhs) const { return lhs.name < rhs.name; } };

class Setting{
  public:
    Setting(const int nb,const std::vector<Role*> rolesArg, const std::vector<int> tasks,const std::vector<int> barrer):nb(nb),roles(rolesArg),tasks(tasks),barrer(barrer){
    }
    const int nb;
    const std::vector<Role*> roles;
    const std::vector<int> tasks;
    const std::vector<int> barrer;
};

std::ostream &operator<<(std::ostream &os, Setting const &setting) { 
  for(auto const & it:setting.roles){
    os << *it <<", ";
  }
  return os;
}

Role * AssassinPtr, * MerlinPtr;
std::map<int,Setting*> settings;
class Avalon{
  public:
    Avalon(int const roomNb):succN(5),failN(5),stats(5),roomNb(roomNb){ };
    std::string setNbPeople(int nb){
      std::ostringstream stream;
      if((nb > 10) or (nb < 5)){
	stream<<"please enter a number between 5 and 10\n";
      } else {
	nbPeople = nb;
	setting=settings[nb];
	rolsThisGame = std::vector<Role*>(setting->roles);
	onTableId2index.clear();
	stream << "we are a "<<nb<<" people game, the rols are: "<<(*setting)<<"\nPlease ask others to join room "<<roomNb<<", and start a new game after everyone join\n";
      }
      return stream.str();
    }
    std::string join(std::string usrname){
      std::ostringstream stream;
      if ( onTableId2index.find(usrname) != onTableId2index.end() ) {
	stream << "you already joined\n";
      } else if (onTableId2index.size() > setting->nb){
	stream<<"you are too late! All " << setting->nb <<" seats are occupied\n";
      } else {
	onTableId2index[usrname] = onTableId2index.size();
	stream<<usrname<<" you have joined successfully ! "<<onTableId2index.size()<<" people joined now\n";
      }
      return stream.str();
    }

    std::string forceNewGame(){
      gameOn = false;
      return newgame();
    }

    std::string newgame(){
      std::ostringstream stream;
      if(onTableId2index.size()<setting->nb){
	stream<<"you need "<<setting->nb<<" people to start the game, while there are only "<<onTableId2index.size()<<" now\n";
      } else if(gameOn){
	stream<<"game is not finished, please don't start a new one now !\n";
      }else{
	shuffle (rolsThisGame.begin(), rolsThisGame.end(), rng);
	role2id.clear();
	int idx=0;
	for (auto const& it : onTableId2index) {
	  role2id[*(rolsThisGame[idx])].push_back(it.first);
	  ++idx;
	}
	whichRound = 0;
	votesNb = 0;
	gameOn = true;
	std::fill(succN.begin(), succN.end(), 0);
	std::fill(failN.begin(), failN.end(), 0);
	std::fill(stats.begin(), stats.end(), "");
	failRdNbs = 0;
	votedThisRound.clear();
	stream<<"game restarted, please ask people to see their profiles\n";
      }
      return stream.str();
    }
    std::string peopleOnTable(){ 
      std::ostringstream stream;
      for (auto const& it : onTableId2index) {
	stream<<it.first<<", ";
      }
      stream<<'\n';
      return stream.str();
    }
    std::string who(std::string usrname){
      std::ostringstream stream;
      if ( onTableId2index.find(usrname) == onTableId2index.end() ) {
	stream<<usrname<<", you havn't joint yet\n";
	return stream.str();
      } 
      auto role = *(rolsThisGame[onTableId2index[usrname]]);
      std::vector<std::string> idsYouKnow;
      for(auto const & it: role.skill){
	idsYouKnow.insert(idsYouKnow.end(), role2id[*it].begin(), role2id[*it].end());
      }
      sort (idsYouKnow.begin(), idsYouKnow.end());
      stream<<usrname<<", you are "<<role<<": "<<role.discription;

      if(role.good){
	for(auto const & it:idsYouKnow){
	  stream<<" "<<it;
	}
      } else {
	for(auto const & it:idsYouKnow){
	  stream<<" "<<it<<":"<<*(rolsThisGame[onTableId2index[it]]);
	}
      }
      stream<<'\n';
      return stream.str();
    }

    std::string progress(std::string usrname){ 
      std::ostringstream stream;
      if(votesNb==0){
	stream<<"  task barrer succN failN stat\n";
	loop(5){
	  stream<<i+1<<"    "<<setting->tasks[i]<<"      "<<setting->barrer[i]<<"     "<<succN[i]<<"     "<<failN[i]<<"    "<<stats[i]<<'\n';
	}
      }else{
	stream<<usrname<<", only "<<votesNb<<" of "<<setting->tasks[whichRound]<<" people voted this round, please wait them to finish\n";
      }
      return stream.str();
    }

    std::string vote(std::string usrname, bool votePass){
      std::ostringstream stream;
      if(not gameOn){
	stream<<usrname<<", you can't vote anymore, game is already over\n";
	return stream.str();
      }
      if(std::find(votedThisRound.begin(), votedThisRound.end(),usrname)!=votedThisRound.end()){
	stream<<usrname<<", you already voted once this round, please don't vote twice !\n";
	return stream.str();
      }
      votedThisRound.push_back(usrname);
      ++votesNb;
      (votePass?succN:failN)[whichRound]++;
      if(votesNb>=setting->tasks[whichRound]){
	bool failed = failN[whichRound]>=setting->barrer[whichRound];
	failRdNbs=failRdNbs+failed;
	stats[whichRound]=failed?"✗":"✓";
	whichRound++;
	votesNb=0;
	votedThisRound.clear();
	if(failRdNbs==3){
	  gameOn=false;
	  stream<<"Well done ! "<<usrname<<" Evil won !\n";
	  return stream.str();
	}
      }
      stream<<usrname<<", your vote is well registred !\n";
      return stream.str();
    }

    std::string assassinate(std::string usrname,std::string name){
      std::ostringstream stream;
      if(not gameOn){
	stream<<usrname<<", game is already over !\n";
	return stream.str();
      }
      auto const & idAssassin = role2id[*AssassinPtr];
      if(std::find(idAssassin.begin(), idAssassin.end(),usrname)==idAssassin.end()){
	stream<<usrname<<", you are not Assassin, you assassinate what ??? Let me recall you something:\n"<<who(usrname);
      }
      gameOn =false;
      if ( onTableId2index.find(name) == onTableId2index.end() ) {
	stream<<name<<" is not on the table, please verify the name\n";
	return stream.str();
      } else {
	auto const & idMerlin = role2id[*MerlinPtr];
	if(std::find(idMerlin.begin(), idMerlin.end(),name)==idMerlin.end()){
	  stream<<"Oh! "<<usrname<<" you assassinated the wrong person ! Good people won !\n";
	}else{
	  stream<<"Well done ! "<<usrname<<" you assassinated the right person ! Evil won !\n";
	}
      }
      return stream.str();
    }
  private:
    int nbPeople;
    Setting* setting;
    std::map<std::string,int> onTableId2index;
    std::map<Role,std::vector<std::string>,RoleCompare> role2id;
    bool gameOn = false;
    std::vector<Role*> rolsThisGame;
    int whichRound = 0;
    int votesNb = 0;
    int failRdNbs = 0;
    int const roomNb;
    std::vector<int>  succN;
    std::vector<int>  failN;
    std::vector<std::string> stats;
    std::vector<std::string> votedThisRound;
    decltype(std::default_random_engine()) rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
};

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

std::vector<Avalon> rooms;
std::string create_room(int nb){
  rooms.push_back(Avalon(rooms.size()));
  return rooms.back().setNbPeople(nb);
}

std::map<std::string,int> usrRoomNb;
std::string respondClient(std::string request){
  const auto& splitedRequest = split(request,',');
  if(splitedRequest.size()<2){
    return "invalid command";
  }
  std::string func = splitedRequest[1];
  auto requestSize = splitedRequest.size();
  if(func=="create_room"){ 
    if(requestSize!=3){error("wrong args");}
    return create_room(std::stoi(splitedRequest[2]));
  }
  const auto & usrname = splitedRequest[0];
  if(func=="join"){ 
    if(requestSize!=3){error("wrong args");}
    int roomNb=std::stoi(splitedRequest[2]);
    if((roomNb>=rooms.size()) or (roomNb<0) ){
      return "Room doesn't exists\n";
    }
    usrRoomNb[usrname] = roomNb;
    return rooms[roomNb].join(usrname);
  }

  if ( usrRoomNb.find(usrname) == usrRoomNb.end() ) {
    return "please join or create a room first\n";
  }
  auto& avalonInstant = rooms[usrRoomNb[usrname]];

  if(func=="forceNewGame"){
    if(requestSize!=2){error("wrong args");}
    return avalonInstant.forceNewGame();
  }
  if(func=="newgame"){ 
    if(requestSize!=2){error("wrong args");}
    return avalonInstant.newgame();
  }
  if(func=="peopleOnTable"){ 
    if(requestSize!=2){error("wrong args");}
    return avalonInstant.peopleOnTable();
  }
  if(func=="who"){ 
    if(requestSize!=2){error("wrong args");}
    return avalonInstant.who(usrname);
  }
  if(func=="progress"){ 
    if(requestSize!=2){error("wrong args");}
    return avalonInstant.progress(usrname);
  }
  if(func=="vote"){ 
    if(requestSize!=3){error("wrong args");}
    return avalonInstant.vote(usrname,splitedRequest[2]=="true");
  }
  if(func=="assassinate"){ 
    if(requestSize!=3){error("wrong args");}
    return avalonInstant.assassinate(usrname,splitedRequest[2]);
  }
  return "";
}


int main(int argc, char *argv[]){
  Role Merlin  ("Merlin",   true,  "Knows evil, must remain hidden, you see");
  Role Percival("Percival", true,  "you see"                                );
  Role Servant ("Servant",  true,  "Loyal servant of Arthur"                );
  Role Assassin("Assassin", false, "Minion of Mordred, you know"            );
  Role Morgana ("Morgana",  false, "you know"                               );
  Role Oberon  ("Oberon",   false, "Unkonwn to Evil"                        );
  Role Mordred ("Mordred",  false, "Unknow to Merlin, you know"             );
  Role Minion  ("Minion",   false, "Minion of Mordred, you know"            );

  Setting setting5 (5,  { &Merlin,&Percival,&Servant,&Morgana,&Assassin                                               },{2,3,2,3,3},{1,1,1,1,1});
  Setting setting6 (6,  { &Merlin,&Percival,&Servant,&Servant,&Morgana,&Assassin                                      },{2,3,4,3,4},{1,1,1,1,1});
  Setting setting7 (7,  { &Merlin,&Percival,&Servant,&Servant,&Morgana,&Assassin,&Oberon                              },{2,3,3,4,4},{1,1,1,2,1});
  Setting setting8 (8,  { &Merlin,&Percival,&Servant,&Servant,&Servant,&Morgana, &Assassin,&Minion                    },{3,4,4,5,5},{1,1,1,2,1});
  Setting setting9 (9,  { &Merlin,&Percival,&Servant,&Servant,&Servant,&Servant, &Morgana, &Mordred,&Assassin         },{3,4,4,5,5},{1,1,1,2,1});
  Setting setting10(10, { &Merlin,&Percival,&Servant,&Servant,&Servant,&Servant, &Morgana, &Mordred,&Assassin,&Oberon },{3,4,4,5,5},{1,1,1,2,1});

  settings[5] =&setting5;
  settings[6] =&setting6;
  settings[7] =&setting7;
  settings[8] =&setting8;
  settings[9] =&setting9;
  settings[10]=&setting10;

  Merlin  .setSkills({&Assassin,&Morgana,&Oberon,&Minion});
  Percival.setSkills({&Merlin,&Morgana});
  Servant .setSkills({});
  Assassin.setSkills({&Morgana,&Mordred,&Minion});
  Morgana .setSkills({&Assassin,&Mordred,&Minion});
  Oberon  .setSkills({});
  Mordred .setSkills({&Assassin,&Morgana,&Minion});
  Minion  .setSkills({&Assassin,&Morgana,&Mordred});

  AssassinPtr = &Assassin;
  MerlinPtr   = &Merlin;

  int sockfd, newsockfd;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  sockfd =  socket(AF_INET, SOCK_STREAM, 0);
  // clear address structure
  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;  
  serv_addr.sin_addr.s_addr = INADDR_ANY;  
  serv_addr.sin_port = htons(atoi(argv[1]));

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");

  // The accept() call actually accepts an incoming connection
  clilen = sizeof(cli_addr);
  listen(sockfd,50);
  while((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))){
    while(true){
      bzero(buffer,256);
      read(newsockfd,buffer,255);
      printf("client: %s\n",buffer);
      auto bufferStr = std::string(buffer);
      if(bufferStr.length()==0){
	break;
      }
      std::string clienStr = respondClient(bufferStr);
      std::cout<<clienStr<<'\n';
      send(newsockfd, clienStr.c_str(), clienStr.length(), 0);
    }
    //close(newsockfd);
    shutdown(newsockfd, SHUT_RDWR);
  }

  close(sockfd);
  return 0; 
}

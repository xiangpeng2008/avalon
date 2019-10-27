#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <math.h>
#include <algorithm> 
#include <fstream>
#include <sstream>
#include <random> 

using namespace std;
typedef int64_t ll;
#define loop(n) for(int64_t i=0;i<n;++i)
#define loop1(n) for(int64_t i=1;i<=n;++i)
#define loopab(a,b) for(int64_t i=a;i<b;++i)
#define max(a,b) a>b?a:b
#define cout(x) cout<<x<<endl;
#define printd(x) printf("%.10f\n",mm);

class Role{
  public:
    Role(const string & name,const bool & good,const string & discription):name(name), good(good),discription(discription){}
    void setSkills(vector<Role*> roles){skill=roles;}
    const string & name;
    const string & discription;
    const bool & good;
    vector<Role*> skill;
};

std::ostream &operator<<(std::ostream &os, Role const &role) { return os << role.name; }

struct RoleCompare { bool operator() (const Role& lhs, const Role& rhs) const { return lhs.name < rhs.name; } };

class Setting{
  public:
    Setting(const int nb,const vector<Role*> rolesArg, const vector<int> tasks,const vector<int> barrer):nb(nb),roles(rolesArg),tasks(tasks),barrer(barrer){
    }
    const int nb;
    const vector<Role*> roles;
    const vector<int> tasks;
    const vector<int> barrer;
};

std::ostream &operator<<(std::ostream &os, Setting const &setting) { 
  for(auto const & it:setting.roles){
    os << *it <<", ";
  }
  return os;
}

//Role Assassin("Assassin", false, "Minion of Mordred, you know"            );
//Role Merlin  ("Merlin",   true,  "Knows evil, must remain hidden, you see");
map<int,Setting*> settings;
class Avalon{
  public:
    Avalon(const Role* AssassinPtr, const Role* MerlinPtr):AssassinPtr(AssassinPtr),MerlinPtr(MerlinPtr),succN(5),failN(5),stats(5){ };
    string setNbPeople(int nb){
      std::ostringstream stream;
      if((nb > 10) or (nb < 5)){
	stream<<"please enter a number between 5 and 10\n";
      } else {
	nbPeople = nb;
	setting=settings[nb];
	rolsThisGame = vector<Role*>(setting->roles);
	onTableId2index.clear();
	stream << "we are a "<<nb<<" people game, the rols are: "<<(*setting)<<"\nPlease ask others to join, and start a new game after everyone join\n";
      }
      return stream.str();
    }
    string join(string usrname){
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

    string newgame(){
      std::ostringstream stream;
      if(onTableId2index.size()<setting->nb){
	stream<<"you need "<<setting->nb<<" people to start the game, while there are only "<<onTableId2index.size()<<" now\n";
      } else if(gameOn){
	stream<<"game is not finished, please don't start a new one now !\n";
      }else{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle (rolsThisGame.begin(), rolsThisGame.end(), std::default_random_engine(seed));

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
    string people_on_table(){ 
      std::ostringstream stream;
      for (auto const& it : onTableId2index) {
	stream<<it.first<<", ";
      }
      stream<<'\n';
      return stream.str();
    }
    string who(string usrname){
      std::ostringstream stream;
      auto role = *(rolsThisGame[onTableId2index[usrname]]);
      vector<string> idsYouKnow;
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
      return stream.str();
    }

    string progress(string usrname){ 
      std::ostringstream stream;
      if(votesNb==0){
	stream<<"task barrer succN failN stat\n";
	loop(5){
	  stream<<i+1<<' '<<setting->tasks[i]<<' '<<setting->barrer[i]<<' '<<succN[i]<<' '<<failN[i]<<' '<<stats[i]<<'\n';
	}
      }else{
	stream<<usrname<<", only "<<votesNb<<" of "<<setting->tasks[whichRound]<<" people voted this round, please wait them to finish\n";
      }
      return stream.str();
    }

    string vote(string usrname, bool votePass){
      std::ostringstream stream;
      if(not gameOn){
	stream<<usrname<<", you can vote anymore, game is already over\n";
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

    string assassinate(string usrname,string name){
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
    map<string,int> onTableId2index;
    map<Role,vector<string>,RoleCompare> role2id;
    bool gameOn = false;
    vector<Role*> rolsThisGame;
    int whichRound = 0;
    int votesNb = 0;
    int failRdNbs = 0;
    vector<int>  succN;
    vector<int>  failN;
    vector<string> stats;
    vector<string> votedThisRound;
    const Role* AssassinPtr;
    const Role* MerlinPtr;
};

int main(){
  Role Merlin  ("Merlin",   true,  "Knows evil, must remain hidden, you see");
  Role Percival("Percival", true,  "you see"                                );
  Role Servant ("Servant",  true,  "Loyal Servant of Arthur"                );
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

  //test case
  //todo add usrname
  Avalon avalon0(&Assassin,&Merlin);
  cout<<avalon0.setNbPeople(5);
  cout<<avalon0.join("a1");
  cout<<avalon0.join("a2");
  cout<<avalon0.join("a3");
  cout<<avalon0.join("a4");
  cout<<avalon0.join("a5");

  return 0;
}


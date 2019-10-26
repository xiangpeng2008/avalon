#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <math.h>
#include <algorithm> 
#include <fstream>
#include <random> 

using namespace std;
typedef int64_t ll;
#define loop(n) for(int64_t i=0;i<n;++i)
#define loop1(n) for(int64_t i=1;i<=n;++i)
#define loopab(a,b) for(int64_t i=a;i<b;++i)
#define max(a,b) a>b?a:b
#define cout(x) cout<<x<<endl;
#define printd(x) printf("%.10f\n",mm);

const double EulerConstant = exp(1.0);

int main(){
  string str;
  int res = 0;
  std::vector<int> data(str.begin(), str.end());
  for(auto & it:data){
    it-=48;
    res +=100*S.avalon[0][0]+10*S.avalon[0][1]+S.avalon[0][2];
  }
  cout<<res<<endl;
  return 0;
}

class Role:{
  public:
    Role(const string & name,bool good,const string & discription):name(name), good(good),discription(discription){}
    void setSkills(vector<Role> roles){skill(roles);}
    const string & name;
    const string & discription;
    const bool good;
    vector<Role> skill;
}

std::ostream &operator<<(std::ostream &os, Role const &role) { 
  return os << role.name;
}

Role Merlin  ("Merlin",   true,  "Knows evil, must remain hidden, you see");
Role Percival("Percival", true,  "you see"                                );
Role Servant ("Servant",  true,  "Loyal Servant of Arthur"                );
Role Assassin("Assassin", false, "Minion of Mordred, you know"            );
Role Morgana ("Morgana",  false, "you know"                               );
Role Oberon  ("Oberon",   false, "Unkonwn to Evil"                        );
Role Mordred ("Mordred",  false, "Unknow to Merlin, you know"             );
Role Minion  ("Minion",   false, "Minion of Mordred, you know"            );

Merlin  .setSkills({Assassin,Morgana,Oberon,Minion});
Percival.setSkills({Merlin,Morgana});
Servant .setSkills({});
Assassin.setSkills({Morgana,Mordred,Minion});
Morgana .setSkills({Assassin,Mordred,Minion});
Oberon  .setSkills({});
Mordred .setSkills({Assassin,Morgana,Minion});
Minion  .setSkills({Assassin,Morgana,Mordred});

class Setting:{
  public:
    Setting(const int nb,const vector<Role> & roles, const vector<int> & tasks,const vector<int> & barrer):nb(nb),roles(roles),tasks(tasks),barrer(barrer){}
    const int nb;
    const vector<Role> & roles;
    const vector<int> & tasks;
    const vector<int> & barrer;
}

std::ostream &operator<<(std::ostream &os, Setting const &setting) { 
  for(auto const & it:setting.roles){
    os << it <<", ";
  }
  return os;
}

Setting setting5 (5,  { Merlin,Percival,Servant,Morgana,Assassin                                        },{2,3,2,3,3},{1,1,1,1,1});
Setting setting6 (6,  { Merlin,Percival,Servant,Servant,Morgana,Assassin                                },{2,3,4,3,4},{1,1,1,1,1});
Setting setting7 (7,  { Merlin,Percival,Servant,Servant,Morgana,Assassin,Oberon                         },{2,3,3,4,4},{1,1,1,2,1});
Setting setting8 (8,  { Merlin,Percival,Servant,Servant,Servant,Morgana,Assassin,Minion                 },{3,4,4,5,5},{1,1,1,2,1});
Setting setting9 (9,  { Merlin,Percival,Servant,Servant,Servant,Servant,Morgana,Mordred,Assassin        },{3,4,4,5,5},{1,1,1,2,1});
Setting setting10(10, { Merlin,Percival,Servant,Servant,Servant,Servant,Morgana,Mordred,Assassin,Oberon },{3,4,4,5,5},{1,1,1,2,1});

map<int,Setting> settings = {
  {5,  setting5 },
  {6,  setting6 },
  {7,  setting7 },
  {8,  setting8 },
  {9,  setting9 },
  {10, setting10}};

class Avalon{
  public:
    Avalon(){ };
    void setNbPeople(int nb){
      if((nb > 10) or (nb < 5)){
	count<<"please enter of number between 5 and 10\n";
      } else {
	nbPeople = nb;
	setting = settings[nb];
	rolsThisGame(setting.roles);
	onTableId2index.clear();
	count<<"we are a "<<nb<<" people game, the rols are: "<<setting<<"\nPlease ask others to join, and start a new game after everyone join\n";
      }
    }
    void join(string usrname){
      if ( onTableId2index.find(usrname) != onTableId2index.end() ) {
	cout<<"you already joined\n";
      } else if (onTableId2index.size() > setting.nb){
	cout<<"you are too late! All " << setting.nb <<" seats are occupied\n";
      } else {
	onTableId2index[usrname] = onTableId2index.size();
	cout<<usrname<<" you have joined successfully ! ", onTableId2index.size()<<" people joined now";
      }
    }

    void newgame(){
      if(onTableId2index.size()<setting.nb){
	cout<<"you need "<<setting.nb<<" people to start the game, while there are only "<<onTableId2index.size()<<" now"`\n";
	return;
      }
      if(gameOn){
	cout<<"game is not finished, please don't start a new one now !\n";
	return;
      }else{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle (rolsThisGame.begin(), rolsThisGame.end(), std::default_random_engine(seed));

	role2id.clear();
	int idx=0;
	for (auto const& it : onTableId2index) {
	  role2id[rolsThisGame[idx]].push_back(it.first);
	  ++idx;
	}
	whichRound = 0;
	votesNb = 0;
	gameOn = true;
	std::fill(succN.begin(), succN.end(), 0);
	std::fill(failN.begin(), failN.end(), 0);
	std::fill(stats.begin(), stats.end(), ' ');
	failRdNbs = 0;
	votedThisRound.clear();
	cout<<"game restarted, please ask people to see their profiles\n";
      }
    }
    void people_on_table(){ 
      for (auto const& it : onTableId2index) {
	cout<<it.first<<", ";
      }
      cout<<'\n';
    }
    void who(string usrname){
      auto role = rolsThisGame[onTableId2index[usrname]];
      vecotr<string> idsYouKnow;
      for(auto const & it: role.skill){
	idsYouKnow.insert(idsYouKnow.end(), role2id[id].begin(), role2id[id].end());
      }
      sort (idsYouKnow.begin(), idsYouKnow.end());
      cout<<usrname<<", you are "<<role<<": "<<role.discription;

      if(role.good){
	for(auto const & it:idsYouKnow){
	  cout<<" "<<it;
	}
      } else {
	for(auto const & it:idsYouKnow){
	  cout<<" "<<it<<":"<<rolsThisGame[onTableId2index[idsYouKnow]];
	}
      }
    }

    void progress(){ 
      if(votesNb==0){
	cout<<"task barrer succN failN stat\n";
	loop(5){
	  cout<<i+1<<' '<<setting.tasks[i]<<' '<<setting.barrer[i]<<' '<<succN[i]<<' '<<failN[i]<<' '<<stats[i]<<'\n';
	}
      }else{
	cout<<usrname<<", only "<<votesNb<<" of "<<setting.tasks[whichRound]<<" people voted this round, please wait them to finish! ";
      }
    }

    void vote(bool votePass){
      if(not gameOn){
	cout<<usrname<<", you can vote anymore, game is already over\n";
	return;
      }
      if(std::find(votedThisRound.begin(), votedThisRound.end(),usrname)!=votedThisRound.end()){
	cout<<usrname<<", you already voted once this round, please don't vote twice !\n";
	return;
      }
      votedThisRound.push_back(usrname);
      ++votesNb;
      (votePass?succN:failN)[whichRound]++;
      if(votesNb>=setting.tasks[whichRound]){
	bool failed = failN[whichRound]>=setting.barrer[whichRound];
	failRdNbs=failRdNbs+failed;
	stats[whichRound]=failed?'✗':'✓';
	whichRound++;
	votesNb=0;
	votedThisRound.clear();
	if(failRdNbs=3){
	  gameOn=false;
	  cout<<"Well done ! "<<usrname<<" Evil won !\n";
	  return;
	}
      }
      cout<<usrname<<", your vote is well registred !\n";
    }

    void assassinate(string usrname,string name){
      if(not gameOn){
	cout<<usrname<<", game is already over !\n";
	return;
      }
      auto const & idAssassin = role2id[Assassin];
      if(std::find(idAssassin.begin(), idAssassin.end(),usrname)==idAssassin.end()){
	cout<<usrname<<", you are not Assassin, you assassinate what ??? Let me recall you something:\n"<<who(usrname);
      }
      gameOn =false;
      if ( onTableId2index.find(name) == onTableId2index.end() ) {
	cout<<name<<" is not on the table, please verify the name\n";
	return;
      } else {
	auto const & idMerlin = role2id[`Merlin];
	if(std::find(idMerlin.begin(), idMerlin.end(),name)==idMerlin.end()){
	  cout<<"Oh! "<<usrname<<" you assassinated the wrong person ! Good people won !\n";
	}else{
	  cout<<"Well done ! "<<usrname<<" you assassinated the right person ! Evil won !\n";
	}
      }
      return;
    }

  private:
    int nbPeople;
    Setting setting;
    map<string,int> onTableId2index;
    map<Role,vector<string>> role2id;
    bool gameOn = false;
    vector<Role> rolsThisGame;
    int whichRound = 0;
    int votesNb = 0;
    int failRdNbs = 0;
    vector<int> succN(5);
    vector<int> failN(5);
    vector<char> stats(5);
    vector<string> votedThisRound;
}


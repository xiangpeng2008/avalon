playersSettings:([nb:(5+til 6)]players:(`Merlin`Percival`Servant`Morgana`Assassin; `Merlin`Percival`Servant`Servant`Morgana`Assassin; `Merlin`Percival`Servant`Servant`Morgana`Assassin`Oberon; `Merlin`Percival`Servant`Servant`Servant`Morgana`Assassin`Minion; `Merlin`Percival`Servant`Servant`Servant`Servant`Mordred`Assassin`Morgana; `Merlin`Percival`Servant`Servant`Servant`Servant`Mordred`Assassin`Morgana`Oberon); missings:(((2 3 2 3 3); (2 3 4 3 4); (2 3 3 4 4)),3#enlist (3 4 4 5 5)); fail_number:((2#enlist 5#1),4#enlist 1 1 1 2 1));
rolesDisSkill:([role:`Merlin`Percival`Servant`Assassin`Morgana`Oberon`Mordred`Minion]discription:("Knows evil, must remain hidden, you see"; "you see"; "Loyal Servant of Arthur"; "Minion of Mordred, you know"; "you know"; "Unkonwn to Evil"; "Unknow to Merlin, you know"; "Minion of Mordred, you know");good:11100000b;skill:(`Assassin`Morgana`Oberon`Minion;`Merlin`Morgana; ();`Morgana`Mordred`Minion;`Assassin`Mordred`Minion;();`Assassin`Morgana`Minion;`Assassin`Morgana`Mordred));
ids:()!();
last_id:();
id2index:()!();
role2id:()!();
missings:();
which_round:1;
votesNb:0;
rols_this_game:();
records:([round:1+til 5]successNb:5#0;failNb:5#0;status:5#`);

set_nb_people:{[nb] 
  $[nb in (0!playersSettings)[`nb]; 
    [`rols_this_game set playersSettings[nb][`players];
      `missings set playersSettings[nb][`missings];
      :raze "`",/:string playersSettings[nb][`players];];
    :"please enter of number between ",(string min (0!playersSettings)[`nb])," and ",(string max (0!playersSettings)[`nb])] };

refresh_players:{`id2index set ()!();};

newgame:{ 
  `rols_this_game set (neg count rols_this_game)?asc rols_this_game;
  `role2id set group (key id2index)!rols_this_game;
  `which_round set 1;
  `votesNb set 0;
  `records set ([round:1+til 5]successNb:5#0;failNb:5#0;status:5#`);
  };

python:{[id;command] 
  if[not " " in command;
    command,:"`";];
  `last_id set `$id; show "python[\"",id,"\";\"",command,"\"]"; :get parse lower command;};

im: {[x] ids[last_id]:x;:"you are ",string x;};

join:{$[(not last_id in key id2index)&((count id2index)<count rols_this_game);
    id2index[last_id]:count id2index;
    show "you are too late!"];};

who:{
  role:rols_this_game[id2index[last_id]];
  idsYouKnow:raze role2id[rolesDisSkill[role;`skill]];
  show res:(string role),": ",rolesDisSkill[role;`discription],raze " ",/:$[rolesDisSkill[role;`good]; string asc ids[idsYouKnow]; (string ids[idsYouKnow]),'":",/:(string rols_this_game[id2index[idsYouKnow]])];
  :res};

vote:{
  `votesNb set votesNb+1;
  s_f_nb:(`success`fail!`successNb`failNb)[x];
  records[which_round;s_f_nb]:records[which_round;s_f_nb]+1;
  if[(get `votesNb)=missings[which_round];
    records[which_round;`status]:$[records[which_round;`failNb]>=missings[which_round];`fail;`sucess];
    which_round+:1;
    `votesNb set 0;];
  }

x:`success

//to add vote

//test
get `records
nbPeople:7;
python["10";"refresh_players"]
python'[string 10+til nbPeople;"im player",/:string 10+til nbPeople]
python["10";"set_nb_people ",string nbPeople];
python[;"join"]'[string 10+til nbPeople]
ids
rols_this_game
python["10";"newgame"];
python[;"who"]'[string 10+til nbPeople];
python[;"who"]'[string 10+til nbPeople];
python["8";"join"];
python["10";"newgame"];
python[;"who"]'[string 10+til nbPeople]

python["10";"vote success"]
python["10";"get records"]
python["10";"get records"]

python["10";"get records"]

get parse "get records"

asc distinct raze (0!playersSettings)`players
`Assassin`Merlin`Minion`Mordred`Morgana`Oberon`Percival`Servant

role:`Merlin
role:`Percival
role:`Assassin
role:`Servant
role:`Morgana
role:`Oberon
(string role),": ",rolesDisSkill[role;`discription],raze " ",/:$[rolesDisSkill[role;`good]; string asc ids[idsYouKnow]; (string ids[idsYouKnow]),'":",/:(string rols_round[id2index[idsYouKnow]])]
python["113000525051104847074";"who"]
python["103112368316136110182";"who"]
who`
`"

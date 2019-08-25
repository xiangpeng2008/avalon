playersSettings:([nb:(5+til 6)]players:(`Merlin`Percival`Servant`Morgana`Assassin; `Merlin`Percival`Servant`Servant`Morgana`Assassin; `Merlin`Percival`Servant`Servant`Morgana`Assassin`Oberon; `Merlin`Percival`Servant`Servant`Servant`Morgana`Assassin`Minion; `Merlin`Percival`Servant`Servant`Servant`Servant`Mordred`Assassin`Morgana; `Merlin`Percival`Servant`Servant`Servant`Servant`Mordred`Assassin`Morgana`Oberon); missings:(((2 3 2 3 3); (2 3 4 3 4); (2 3 3 4 4)),3#enlist (3 4 4 5 5)); fail_number:((2#enlist 5#1),4#enlist 1 1 1 2 1));
rolesDisSkill:([role:`Merlin`Percival`Servant`Assassin`Morgana`Oberon`Mordred`Minion]discription:("Knows evil, must remain hidden, you see"; "you see"; "Loyal Servant of Arthur"; "Minion of Mordred, you know"; "you know"; "Unkonwn to Evil"; "Unknow to Merlin, you know"; "Minion of Mordred, you know");good:11100000b;skill:(`Assassin`Morgana`Oberon`Minion;`Merlin`Morgana; ();`Morgana`Mordred`Minion;`Assassin`Mordred`Minion;();`Assassin`Morgana`Minion;`Assassin`Morgana`Mordred));

id2names:()!();
last_id:();
onTableId2index:()!();
role2id:()!();
missings:();
last_name:"";
rols_this_game:();
records_init:([round:1+til 5]successNb:5#0;failNb:5#0;status:5#`);

voted_this_round:();
votesNb:0;
which_round:1;
records:records_init;

set_nb_people:{[nb] 
  nb:"J"$nb;
  $[nb in (0!playersSettings)[`nb]; 
    [`rols_this_game set playersSettings[nb][`players];
      `missings set flip `missings`fail_number # playersSettings[nb];
      refresh_players`;
      :"we are a ",(string nb)," people game, the rols are",(raze " ",/:string rols_this_game),".\nPlease ask others to join, and start a new game after everyone join";];
    :"please enter of number between ",(string min (0!playersSettings)[`nb])," and ",(string max (0!playersSettings)[`nb])];
  };

refresh_players:{`onTableId2index set ()!();:"Done, please ask people to re-join";};

newgame:{ 
  `rols_this_game set (neg count rols_this_game)?asc rols_this_game;
  `role2id set group (key onTableId2index)!rols_this_game;
  `which_round set 1;
  `votesNb set 0;
  `records set records_init;
  `voted_this_round set ();
  :"game restarted, please ask people to see their profiles";
  };

python:{[id;command] 
  `last_id set `$id; 
  `last_name set id2names[last_id];
  :get lower command;};

im: {[x] id2names[last_id]:`$x;:"you are ",x;};

join:{
  if[last_id in key onTableId2index; :"you already joined";];
  if[((count onTableId2index)>=count rols_this_game); :"you are too late! All ",(string count rols_this_game)," seats are occupied";];
  onTableId2index[last_id]:count onTableId2index;
  :(string last_name)," you have joined successfully ! ", (string count onTableId2index)," people joined now";
  };

who:{
  role:rols_this_game[onTableId2index[last_id]];
  idsYouKnow:raze role2id[rolesDisSkill[role;`skill]];
  show res:(string last_name),", you are ",(string role),": ",rolesDisSkill[role;`discription],raze " ",/:$[rolesDisSkill[role;`good]; string asc id2names[idsYouKnow]; (string id2names[idsYouKnow]),'":",/:(string rols_this_game[onTableId2index[idsYouKnow]])];
  :res};

vote:{
  if[last_id in voted_this_round;
    :(string last_name),", you already voted once this round, please don't vote twice !";
    ];
  `voted_this_round set voted_this_round,last_id;
  `votesNb set votesNb+1;
  s_f_nb:(`success`fail!`successNb`failNb)[x];
  records[which_round;s_f_nb]:records[which_round;s_f_nb]+1;
  if[votesNb>=missings[which_round-1;`missings];
    records[which_round;`status]:`$$[records[which_round;`failNb]>=missings[which_round-1;`fail_number];"✗";"✓"];
    `which_round set which_round+1;
    `votesNb set 0;
    `voted_this_round set ();
    ];
  :(string last_name),", your vote is well registred !";
  }

assassinate:{
  if[not last_id in role2id[`Assassin];
    :(string last_name),", you are not Assassin, you assassinate what ??? Let me recall you something:\n",who`;
    ];
  name:`$x;
  id4name:id2names?name;
  if[null id4name; :x," is not on the table, please verify the name";];
  $[id4name in role2id[`Merlin];
    :"Well done ! ",(string last_name)," you assassinated the right person ! Evil won !";
    :"Oh! ",(string last_name)," you assassinated the wrong person !"
    ];
  }

//test
nbPeople:7;
python["103112368316136110182";"refresh_players`"]
python'[string 10+til nbPeople;"im[\"player",/:(string 10+til nbPeople),\:"\"]"]
python["103112368316136110182";"im[\"xiangpeng\"]"]
python["103112368316136110182";"set_nb_people ",string nbPeople]
python["103112368316136110182";"join`"]
python["103112368316136110182";"value id2names"]
python[;"join`"]'[string 10+til nbPeople]
python["103112368316136110182";"newgame`"]
python["103112368316136110182";"who`"]
python[;"who`"]'[string 10+til nbPeople]
python["8";"join`"]
python["103112368316136110182";"newgame"];
python[;"who`"]'[string 10+til nbPeople]
python["103112368316136110182";"records"]


python["103112368316136110182";"vote success"]
python["103112368316136110182";"get records"]
python["103112368316136110182";"get records"]
python["103112368316136110182";"get records"]
missings
parse "get records "
parse "get "

get parse "value id2names"
get parse "value[id2names]"
parse "value[id2names]"
get "value[value`id2names]"
get parse "value[value`id2names]"
get parse"value[value`id2names]"
get parse "key id2names"

asc distinct raze (0!playersSettings)`players
`Assassin`Merlin`Minion`Mordred`Morgana`Oberon`Percival`Servant

role:`Merlin
role:`Percival
role:`Assassin
role:`Servant
role:`Morgana
role:`Oberon
(string role),": ",rolesDisSkill[role;`discription],raze " ",/:$[rolesDisSkill[role;`good]; string asc id2names[idsYouKnow]; (string id2names[idsYouKnow]),'":",/:(string rols_round[onTableId2index[idsYouKnow]])]
python["113000525051104847074";"who"]
python["103112368316136110182";"who"]
who`

`missings set flip `missings`fail_number # playersSettings[7]
[`missings]
type records_init[0;`status]
`"

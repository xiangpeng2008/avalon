playersSettings:([nb:(5+til 6)]players:(`Merlin`Percival`Servant`Morgana`Assassin; `Merlin`Percival`Servant`Servant`Morgana`Assassin; `Merlin`Percival`Servant`Servant`Morgana`Assassin`Oberon; `Merlin`Percival`Servant`Servant`Servant`Morgana`Assassin`Minion; `Merlin`Percival`Servant`Servant`Servant`Servant`Mordred`Assassin`Morgana; `Merlin`Percival`Servant`Servant`Servant`Servant`Mordred`Assassin`Morgana`Oberon); tasks:(((2 3 2 3 3); (2 3 4 3 4); (2 3 3 4 4)),3#enlist (3 4 4 5 5)); barrer:((2#enlist 5#1),4#enlist 1 1 1 2 1));
rolesDisSkill:([role:`Merlin`Percival`Servant`Assassin`Morgana`Oberon`Mordred`Minion]discription:("Knows evil, must remain hidden, you see"; "you see"; "Loyal Servant of Arthur"; "Minion of Mordred, you know"; "you know"; "Unkonwn to Evil"; "Unknow to Merlin, you know"; "Minion of Mordred, you know");good:11100000b;skill:(`Assassin`Morgana`Oberon`Minion;`Merlin`Morgana; ();`Morgana`Mordred`Minion;`Assassin`Mordred`Minion;();`Assassin`Morgana`Minion;`Assassin`Morgana`Mordred));

last_id:();
onTableId2index:()!();
role2id:()!();
tasks:();
rols_this_game:();
records_init:([round:1+til 5]succN:5#0;failN:5#0;stat:5#`);

voted_this_round:();
votesNb:0;
which_round:1;
records:();
gameOn:0b;
failRdNbs:0;

set_nb_people:{[nb] 
  nb:"J"$nb;
  $[nb in (0!playersSettings)[`nb]; 
    [`rols_this_game set playersSettings[nb][`players];
      `tasks set flip `tasks`barrer # playersSettings[nb];
      `onTableId2index set ()!();
      :"we are a ",(string nb)," people game, the rols are",(raze " ",/:string rols_this_game),".\nPlease ask others to join, and start a new game after everyone join";];
    :"please enter of number between ",(string min (0!playersSettings)[`nb])," and ",(string max (0!playersSettings)[`nb])];
  };

progress:{
  $[votesNb=0;:records;:(string last_id),", only ",(string votesNb)," of ",(string records[which_round;`tasks])," people voted this round, please wait them to finish! "]; };

newgame:{ 
  if[gameOn;
    :"game is not finished, please don't start a new one now !";];
  `rols_this_game set (neg count rols_this_game)?asc rols_this_game;
  `role2id set group (key onTableId2index)!rols_this_game;
  `which_round set 1;
  `votesNb set 0;
  `gameOn set 1b;
  `failRdNbs set 0;
  `records set tasks,'records_init;
  `voted_this_round set ();
  :"game restarted, please ask people to see their profiles";
  };

python:{[id;command] 
  `last_id set `$id; 
  show res:get lower command;
  :res;
  };

join:{
  if[last_id in key onTableId2index; :"you already joined";];
  if[((count onTableId2index)>=count rols_this_game); :"you are too late! All ",(string count rols_this_game)," seats are occupied";];
  onTableId2index[last_id]:count onTableId2index;
  :(string last_id)," you have joined successfully ! ", (string count onTableId2index)," people joined now";
  };

people_on_table:{ :string key onTableId2index; };

who:{
  role:rols_this_game[onTableId2index[last_id]];
  idsYouKnow:raze role2id[rolesDisSkill[role;`skill]];
  :(string last_id),", you are ",(string role),": ",rolesDisSkill[role;`discription],raze " ",/:$[rolesDisSkill[role;`good]; string asc idsYouKnow; (string idsYouKnow),'":",/:(string rols_this_game[onTableId2index[idsYouKnow]])]; };

vote:{
  if[not gameOn;:(string last_id),", you can vote anymore, game is already over!"];
  if[last_id in voted_this_round;
    :(string last_id),", you already voted once this round, please don't vote twice !";
    ];
  `voted_this_round set voted_this_round,last_id;
  `votesNb set votesNb+1;
  s_f_nb:(`success`fail!`succN`failN)[x];
  records[which_round;s_f_nb]:records[which_round;s_f_nb]+1;
  if[votesNb>=records[which_round;`tasks];
    records[which_round;`stat]:`$$[records[which_round;`failN]>=records[which_round;`barrer];[`failRdNbs set failRdNbs+1;"✗"];"✓"];
    `which_round set which_round+1;
    `votesNb set 0;
    `voted_this_round set ();
    if[failRdNbs=3;
      `gameOn set 0b;
      :"Well done ! ",(string last_id)," Evil won !";];
    ];
  :(string last_id),", your vote is well registred !";
  }

assassinate:{
  if[not gameOn; :(string last_id),", game is already over !";];
  `gameOn set 0b;
  if[not last_id in role2id[`Assassin];
    :(string last_id),", you are not Assassin, you assassinate what ??? Let me recall you something:\n",who`;
    ];
  name:`$x;
  id4name:id2names?name;
  if[null id4name; :x," is not on the table, please verify the name";];
  $[id4name in role2id[`Merlin];
    :"Well done ! ",(string last_id)," you assassinated the right person ! Evil won !";
    :"Oh! ",(string last_id)," you assassinated the wrong person ! Good people won !"
    ];
  }


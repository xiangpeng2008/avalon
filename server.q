players:(5+til 6)! (`MERLIN`PERCIVAL`SERVANT`MORGANA`ASSASSIN; `MERLIN`PERCIVAL`SERVANT`SERVANT`MORGANA`ASSASSIN; `MERLIN`PERCIVAL`SERVANT`SERVANT`MORGANA`ASSASSIN`OBERON; `MERLIN`PERCIVAL`SERVANT`SERVANT`SERVANT`MORGANA`ASSASSIN`MINION; `MERLIN`PERCIVAL`SERVANT`SERVANT`SERVANT`SERVANT`MORDRED`ASSASSIN`MORGANA; `MERLIN`PERCIVAL`SERVANT`SERVANT`SERVANT`SERVANT`MORDRED`ASSASSIN`MORGANA`OBERON)
rols:`MERLIN`PERCIVAL`SERVANT`ASSASSIN`MORGANA`OBERON`MORDRED`MINION!("Knows evil, must remain hidden"; "Knows Merlin"; "Loyal Servant of Arthur"; "Minion of Mordred"; "Appears as Merlin"; "Unkonwn to Evil"; "Unknow to Merlin"; "Minion of Mordred");

creat:{[nb] $[nb in key players; [`rols_round set (neg nb)?players[nb];:raze "`",/:string players[nb];];:"please enter of number between ",(string min key players)," and ",(string max key players)] };
shuffle:{ `rols_round set (neg count rols_round)?rols_round};


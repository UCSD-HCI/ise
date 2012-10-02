using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RiskSpace
{
    public class PlayerManager
    {
        public const int PlayerNum = 2;
        private Player[] players;
        private RiskMap map;

        public PlayerManager(RiskMap map)
        {
            this.map = map;
            players = new Player[PlayerNum];
            for (int i = 0; i < PlayerNum; i++)
            {
                players[i] = new Player(i + 1);
            }
        }

        public Player GetPlayer(int playerId)
        {
            return players[playerId - 1];
        }

        public void Update()
        {
            for (int i = 0; i < PlayerNum; i++)
            {
                players[i].ArmyNum = 0;
                players[i].CountryNum = 0;
            }

            foreach (ArmyViz armyViz in map.GetAllArmyViz())
            {
                if (armyViz.PlayerId == 0)
                {
                    continue;
                }

                players[armyViz.PlayerId - 1].CountryNum++;
                players[armyViz.PlayerId - 1].ArmyNum += armyViz.ArmyCount;
            }
        }

    }
}

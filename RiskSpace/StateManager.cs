using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace RiskSpace
{
    public enum GameState
    {
        Invalid = 0,

        ChooseCountry,

        //TODO: use cards
        AddArmy,

        AttackChooseSource,
        AttackPickArmy,
        AttackChooseDest,
        AttackWaitDice,
        AttackAnimation,

        //TODO: rearrange
        //TODO: pick cards
        
    }

    public class StateManager
    {
        readonly int[] CardSetToArmy = new int[] { 4, 6, 8, 10, 12, 15, 20, 25 };

        private PlayerManager playerManager;

        public GameState State { get; private set; }
        public int ActivePlayerId { get; private set; }
        public int RoundCount { get; private set; }
        public int AvailabeNewArmy { get; private set; }
        public int MovableArmy { get; private set; }
        public bool IsErrored { get; private set; }
        public string AttackSource {get; private set;}
        public string AttackDest {get; private set;}
        //public List<Dice> Dices { get; private set; }
        public bool IsAnimating { get; set; }
        public int AttackerMaxDiceNum { get; private set; }
        public int DefenderMaxDiceNum { get; private set; }
        public int AttackerLost { get; private set; }
        public int DefenderLost { get; private set; }
        public bool IsAttackerWin { get; private set; }
        public int CardSet { get; private set; }

        public StateManager(PlayerManager playerManager)
        {
            State = GameState.ChooseCountry;
            ActivePlayerId = 1;
            RoundCount = 0;
            CardSet = 0;
            this.playerManager = playerManager;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>If it's already the last player, return false; otherwise return true </returns>
        private bool nextPlayer()
        {
            if (ActivePlayerId == PlayerManager.PlayerNum)
            {
                ActivePlayerId = 1;
                return false;
            }
            else
            {
                ActivePlayerId++;
                return true;
            }
        }

        public void Next()
        {
            switch (State)
            {
                case GameState.ChooseCountry:
                    nextPlayer();
                    break;

                case GameState.AddArmy:
                    AvailabeNewArmy--;
                    if (AvailabeNewArmy == 0)
                    {
                        State = GameState.AttackChooseSource;
                    }
                    break;

                case GameState.AttackPickArmy:
                    break;

                default:
                    Debug.Assert(false);
                    break;
            }

            IsErrored = false;
            refreshPlayers();
        }

        public void Finish()
        {
            switch (State)
            {
                case GameState.ChooseCountry:
                    ActivePlayerId = 1;
                    State = GameState.AddArmy;
                    refreshAvailabeNewArmy();
                    break;

                case GameState.AttackChooseSource:
                    State = GameState.AttackChooseDest;
                    break;

                case GameState.AttackChooseDest:
                    State = GameState.AttackWaitDice;
                    break;

                case GameState.AttackWaitDice:
                    State = GameState.AttackAnimation;
                    break;

                case GameState.AttackAnimation:
                    //State = MovableArmy > 0 ? GameState.AttackPickArmy : GameState.AttackChooseSource;
                    State = GameState.AttackChooseSource;
                    break;

                case GameState.AttackPickArmy:
                    Debug.Assert(false);    //TODO
                    State = GameState.AttackChooseSource;
                    break;

                default:
                    Debug.Assert(false);
                    break;
            }

            IsErrored = false;
            refreshPlayers();
        }

        public void Cancel()
        {
            switch (State)
            {
                case GameState.AttackChooseDest:
                    State = GameState.AttackChooseSource;
                    break;

                case GameState.AttackWaitDice:
                    State = GameState.AttackChooseSource;
                    break;

                default:
                    Debug.Assert(false);
                    break;
            }

            IsErrored = false;
            refreshPlayers();
        }

        public void RoundPass()
        {
            Debug.Assert(State == GameState.AttackChooseSource);
            refreshAvailabeNewArmy();
            State = GameState.AddArmy;
            if (!nextPlayer())
            {
                RoundCount++;
            }
            IsErrored = false;
        }

        public void Error()
        {
            switch (State)
            {
                case GameState.AttackWaitDice:
                    break;

                default:
                    Debug.Assert(false);
                    break;
            }

            IsErrored = true;
        }

        public void AttackSourceSelected(string sourceId, int attackerMaxDiceNum)
        {
            Debug.Assert(State == GameState.AttackChooseSource);
            this.AttackSource = sourceId;
            this.AttackerMaxDiceNum = attackerMaxDiceNum;
            Finish();
        }

        public void AttackDestSelected(string destId, int defenderMaxDiceNum)
        {
            Debug.Assert(State == GameState.AttackChooseDest);
            this.AttackDest = destId;
            this.DefenderMaxDiceNum = defenderMaxDiceNum;
            Finish();
        }

        public void AttackDiceDetected(int attackerLost, int defenderLost, bool attackerWin)
        {
            Debug.Assert(State == GameState.AttackWaitDice);
            this.AttackerLost = attackerLost;
            this.DefenderLost = defenderLost;
            this.IsAttackerWin = attackerWin;
            Finish();
        }

        public void AddNewArmyByCard()
        {
            int newArmy;
            if (CardSet < CardSetToArmy.Length)
            {
                newArmy = CardSetToArmy[CardSet];
            }
            else
            {
                newArmy = CardSetToArmy.Last() + (CardSet - CardSetToArmy.Length + 1) * 5;
            }

            AvailabeNewArmy += newArmy;
            CardSet++;
        }

        /*public void Win(int movableArmy)
        {
            Debug.Assert(State == GameState.AttackAnimation);
            this.MovableArmy = movableArmy;
            Finish();
        }

        public void Lose(int movableArmy)
        {
            Debug.Assert(State == GameState.AttackAnimation);
            this.MovableArmy = 0;
            Finish();
        }*/

        private void refreshPlayers()
        {
            playerManager.Update();
        }

        private void refreshAvailabeNewArmy()
        {
            //AvailabeNewArmy = Math.Max(3, (int)(playerManager.GetPlayer(ActivePlayerId).CountryNum / 1.5));
            AvailabeNewArmy = 25;
        }
    }
}

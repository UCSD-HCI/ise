using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;
using System.Media;

using Multitouch.Framework.WPF.Input;
using InteractiveSpace.SDK;

namespace RiskSpace
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Multitouch.Framework.WPF.Controls.Window
    {
        private StateManager stateManager;
        private PlayerManager playerManager;
        private WizardOzWindow wizardOzWindow;
        private GameState previousState = GameState.Invalid;
        private SoundPool soundPool;

        private InteractiveSpaceProvider interactiveSpaceProvider;

        public MainWindow()
        {
            interactiveSpaceProvider = new InteractiveSpace.SDK.DLL.InteractiveSpaceProviderDLL();
            interactiveSpaceProvider.Connect();

            InitializeComponent();

            MultitouchScreen.AllowNonContactEvents = true;

            playerManager = new PlayerManager(riskMap);
            stateManager = new StateManager(playerManager);

            wizardOzWindow = new WizardOzWindow();
            wizardOzWindow.DiceLocated += new EventHandler<DiceEventArgs>(wizardOzWindow_DiceLocated);
            wizardOzWindow.DiceFinished += new EventHandler<DiceFinishedEventArgs>(wizardOzWindow_DiceFinished);
            wizardOzWindow.CardLocated += new EventHandler<CardEventArgs>(wizardOzWindow_CardLocated);
            wizardOzWindow.CardFinished += new EventHandler<CardFinishedEventArgs>(wizardOzWindow_CardFinished);
            wizardOzWindow.Show();

            soundPool = new SoundPool();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            msgControl.UpdateMessage(stateManager, playerManager, riskMap);

            soundPool.PlayBgm();
        }

        private void riskMap_CountryClick(object sender, CountryContactEventArgs e)
        {
            switch (stateManager.State)
            {
                case GameState.ChooseCountry:
                    chooseCountry(e.CountryId);
                    break;

                case GameState.AddArmy:
                    addArmy(e.CountryId);
                    break;

                case GameState.AttackChooseSource:
                    attackChooseSource(e.CountryId);
                    break;

                case GameState.AttackChooseDest:
                    attackChooseDest(e.CountryId);
                    break;

                default:
                    break;
            }
        }

        private void refreshViews()
        {
            playerInfo1.Update(playerManager.GetPlayer(1), stateManager.ActivePlayerId == 1);
            playerInfo2.Update(playerManager.GetPlayer(2), stateManager.ActivePlayerId == 2);

            //udpate messages
            msgControl.UpdateMessage(stateManager, playerManager, riskMap);

            //update buttons
            ControlButton activeButtons = stateManager.ActivePlayerId == 1 ? controlButton1 : controlButton2;
            ControlButton inactiveButtons = stateManager.ActivePlayerId == 1 ? controlButton2 : controlButton1;

            activeButtons.HideAllButtons();
            switch (stateManager.State)
            {
                case GameState.AttackChooseSource:
                    activeButtons.NextButtonVisible = true;
                    break;

                case GameState.AttackChooseDest:
                case GameState.AttackWaitDice:
                    activeButtons.CancelButtonVisible = true;
                    break;
                    
                default:
                    activeButtons.HideAllButtons();
                    break;
            }

            inactiveButtons.HideAllButtons();

            //stop attacking
            if (stateManager.State == GameState.AttackChooseSource && stateManager.AttackSource != null)
            {
                riskMap.GetArmyViz(stateManager.AttackSource).IsAttacking = false;
            }

            //attack line
            if (stateManager.State == GameState.AttackWaitDice && !stateManager.IsErrored)
            {
                riskMap.DrawAttackLine(stateManager.AttackSource, stateManager.AttackDest);
            }
            else if (stateManager.State != GameState.AttackWaitDice)
            {
                riskMap.HideAttackLine();
            }

            //wizardOz
            if (stateManager.State == GameState.AttackWaitDice)
            {
                wizardOzWindow.RequestDice();
            }

            //dice viz
            if (stateManager.State != GameState.AttackWaitDice || stateManager.IsErrored)
            {
                diceViz.ClearDices();
            }

            //card
            CardHolder cardHolder = stateManager.ActivePlayerId == 1 ? cardHolder1 : cardHolder2;
            if (stateManager.State == GameState.AddArmy && previousState != GameState.AddArmy)
            {
                cardHolder.Show();
                wizardOzWindow.RequestCard();
            }
            else if (stateManager.State == GameState.AttackChooseSource)
            {
                cardHolder.Hide();
                wizardOzWindow.CancelCard();
            }

            previousState = stateManager.State;
        }

        private void chooseCountry(string countryId)
        {
            ArmyViz armyViz = riskMap.GetArmyViz(countryId);

            if (armyViz.PlayerId != 0)
            {
                return;
            }

            armyViz.PlayerId = stateManager.ActivePlayerId;
            armyViz.AddArmy();

            //check if all countries are picked
            int blankCount = 0;
            foreach (ArmyViz eachArmyViz in riskMap.GetAllArmyViz())
            {
                if (eachArmyViz.PlayerId == 0)
                {
                    blankCount++;
                    armyViz = eachArmyViz;

                    if (blankCount >= 2)
                    {
                        break;
                    }
                }
            }

            if (blankCount == 1)
            {
                armyViz.PlayerId = stateManager.ActivePlayerId == PlayerManager.PlayerNum ? 1 : stateManager.ActivePlayerId + 1;
                armyViz.AddArmy();
                stateManager.Finish();
            }
            else
            {
                stateManager.Next();
            }               
                
            refreshViews();
            soundPool.Play(GameSound.AddArmy);
            
        }

        private void addArmy(string countryId)
        {
            ArmyViz armyViz = riskMap.GetArmyViz(countryId);
            if (riskMap.GetArmyViz(countryId).PlayerId != stateManager.ActivePlayerId)
            {
                return;
            }

            armyViz.AddArmy();
            stateManager.Next();
            refreshViews();
            soundPool.Play(GameSound.AddArmy);
        }

        private void attackChooseSource(string countryId)
        {
            ArmyViz armyViz = riskMap.GetArmyViz(countryId);
            if (armyViz.PlayerId != stateManager.ActivePlayerId || armyViz.ArmyCount < 2 )
            {
                return;
            }

            /*attackArmyViz.CountryID = countryId;
            attackArmyViz.PlayerId = stateManager.ActivePlayerId;
            attackArmyViz.AddArmy();

            armyViz.ArmyCount--;

            Point p = armyViz.TranslatePoint(new Point(0, 0), mainGrid);
            
            //Size vizSize = armyViz.RenderSize;
            //p -= new Vector(vizSize.Width, vizSize.Height);

            p -= new Vector(50, 50);
            attackArmyViz.Margin = new Thickness(p.X, p.Y, 0, 0);*/

            armyViz.IsAttacking = true;
            int maxDiceNum = Math.Min(3, armyViz.ArmyCount - 1);
            stateManager.AttackSourceSelected(countryId, maxDiceNum);
            //attackArmyViz.Visibility = Visibility.Visible;

            refreshViews();
            soundPool.Play(GameSound.AttackChooseSource);
        }

        private void attackChooseDest(string countryId)
        {
            ArmyViz armyViz = riskMap.GetArmyViz(countryId);
            if (armyViz.PlayerId == stateManager.ActivePlayerId)
            {
                return;
            }

            int maxDiceNum = Math.Min(2, armyViz.ArmyCount);
            stateManager.AttackDestSelected(countryId, maxDiceNum);
            refreshViews();

            ArmyViz attacker = riskMap.GetArmyViz(stateManager.AttackSource);
            if (attacker.ArmyCount < 5)
            {
                soundPool.Play(GameSound.AttackL1);
            }
            else if (attacker.ArmyCount < 10)
            {
                soundPool.Play(GameSound.AttackL2);
            }
            else
            {
                soundPool.Play(GameSound.AttackL3);
            }
        }

        private void controlButton_OkButtonClick(object sender, EventArgs e)
        {
            
        }

        private void controlButton_CancelButtonClick(object sender, EventArgs e)
        {
            switch (stateManager.State)
            {
                case GameState.AttackChooseDest:
                case GameState.AttackWaitDice:
                    stateManager.Cancel();
                    break;
            }

            refreshViews();
        }

        private void controlButton_NextButtonClick(object sender, EventArgs e)
        {
            switch (stateManager.State)
            {
                case GameState.AttackChooseSource:
                    stateManager.RoundPass();
                    soundPool.Play(GameSound.RoundPass);
                    break;

                default:
                    Debug.Assert(false);
                    break;
            }

            refreshViews();
        }

        private void wizardOzWindow_DiceLocated(object sender, DiceEventArgs e)
        {
            Debug.Assert(stateManager.State == GameState.AttackWaitDice);
            diceViz.ShowDice(e.Dice);
            soundPool.Play(GameSound.Success);
        }


        private void wizardOzWindow_DiceFinished(object sender, DiceFinishedEventArgs e)
        {
            Debug.Assert(stateManager.State == GameState.AttackWaitDice);

            var whiteDices = from g in e.AllDices
                             where g.DiceColor == DiceColor.White
                             orderby g.Value descending
                             select g;

            var redDices = from g in e.AllDices
                           where g.DiceColor == DiceColor.Red
                           orderby g.Value descending
                           select g;

            if (redDices.Count() == 0 || redDices.Count() > stateManager.AttackerMaxDiceNum
                || whiteDices.Count() != stateManager.DefenderMaxDiceNum)
            {
                stateManager.Error();
                soundPool.Play(GameSound.Error);
            }
            else
            {
                //judgement
                ArmyViz attackerViz = riskMap.GetArmyViz(stateManager.AttackSource);
                ArmyViz defenderViz = riskMap.GetArmyViz(stateManager.AttackDest);
                var redIt = redDices.GetEnumerator();
                var whiteIt = whiteDices.GetEnumerator();
                int attackerLost = 0, defenderLost = 0;

                while (redIt.MoveNext() && whiteIt.MoveNext())
                {
                    if (redIt.Current.Value > whiteIt.Current.Value)
                    {
                        defenderLost++;
                    }
                    else
                    {
                        attackerLost++;
                    }
                }

                Debug.Assert(attackerViz.ArmyCount >= attackerLost + 1);
                Debug.Assert(defenderViz.ArmyCount >= defenderLost);

                bool attackerWin;
                //owner changing
                if (defenderViz.ArmyCount == defenderLost)  //win
                {
                    defenderViz.PlayerId = stateManager.ActivePlayerId;
                    defenderViz.ArmyCount = redDices.Count() - attackerLost;
                    attackerViz.ArmyCount -= redDices.Count();
                    attackerWin = true;
                }
                else    //lose
                {
                    attackerViz.ArmyCount -= attackerLost;
                    defenderViz.ArmyCount -= defenderLost;
                    attackerWin = false;
                }

                stateManager.AttackDiceDetected(attackerLost, defenderLost, attackerWin);

                if (attackerWin)
                {
                    soundPool.Play(GameSound.AttackWin);
                }
                else
                {
                    soundPool.Play(GameSound.AttackFail);
                }
            }

            refreshViews();
        }

        void wizardOzWindow_CardFinished(object sender, CardFinishedEventArgs e)
        {
            Debug.Assert(stateManager.State == GameState.AddArmy);
            CardHolder cardHolder = stateManager.ActivePlayerId == 1 ? cardHolder1 : cardHolder2;
            cardHolder.MatchFinished(e.IsAccepted);

            if (e.IsAccepted)
            {
                stateManager.AddNewArmyByCard();
                refreshViews();
                soundPool.Play(GameSound.Success);
            }
            else
            {
                wizardOzWindow.RequestCard();
                soundPool.Play(GameSound.Error);
            }
        }

        void wizardOzWindow_CardLocated(object sender, CardEventArgs e)
        {
            Debug.Assert(stateManager.State == GameState.AddArmy);
            CardHolder cardHolder = stateManager.ActivePlayerId == 1 ? cardHolder1 : cardHolder2;
            cardHolder.LocateCard(e.CardId);
        }

        private void msgControl_AnimationCompleted(object sender, EventArgs e)
        {
            if (stateManager.State == GameState.AttackAnimation)
            {
                stateManager.Finish();
                refreshViews();
            }
        }
    }
}

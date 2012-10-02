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
using System.Windows.Media.Animation;
using System.Media;

namespace RiskSpace
{
    /// <summary>
    /// Interaction logic for GameMessageControl.xaml
    /// </summary>
    public partial class GameMessageControl : UserControl
    {
        private StateManager stateManager;

        public event EventHandler AnimationCompleted;

        public GameMessageControl()
        {
            InitializeComponent();
        }

        public void UpdateMessage(StateManager stateManager, PlayerManager playerManager, RiskMap map)
        {
            this.stateManager = stateManager;   //TODO: stupid...

            string msg;
            switch (stateManager.State)
            {
                case GameState.ChooseCountry:
                    msg = "Pick one country. ";
                    break;

                case GameState.AddArmy:
                    msg = "Add your army! (" + stateManager.AvailabeNewArmy.ToString() + " left)";
                    break;

                case GameState.AttackChooseSource:
                    msg = "Choose your army to attack! ";
                    break;

                case GameState.AttackChooseDest:
                    msg = "Where will you attack? ";
                    break;

                case GameState.AttackPickArmy:
                    msg = "Pick more army to defense your new territory!";
                    break;

                default:
                    msg = null;
                    break;
            }

            if (stateManager.State == GameState.AttackWaitDice)
            {
                Player attacker = playerManager.GetPlayer(stateManager.ActivePlayerId);
                Player defender = playerManager.GetPlayer(map.GetArmyViz(stateManager.AttackDest).PlayerId);
                string attackerDice = stateManager.AttackerMaxDiceNum > 1 ? "dices" : "dice";
                string defenderDice = stateManager.DefenderMaxDiceNum > 1 ? "dices" : "dice";
                msgTextBlock.Text = string.Format("Player {0}: Throw {1} {2}! \n Player {3}: Throw {4} {5}! ", 
                    attacker.PlayerId, 
                    stateManager.AttackerMaxDiceNum, 
                    attackerDice,
                    defender.PlayerId, 
                    stateManager.DefenderMaxDiceNum,
                    defenderDice);
                msgTextBlock.Foreground = new SolidColorBrush(attacker.MainColor);
            }
            else if (stateManager.State == GameState.AttackAnimation)
            {
                Player attacker = playerManager.GetPlayer(stateManager.ActivePlayerId);
                Player defender = playerManager.GetPlayer(map.GetArmyViz(stateManager.AttackDest).PlayerId);
                msgTextBlock.Text = string.Format("Player {0}: {1} Lost; Player {2}: {3} Lost. \n Battle {4}!",
                    attacker.PlayerId,
                    stateManager.AttackerLost,
                    defender.PlayerId,
                    stateManager.DefenderLost,
                    stateManager.IsAttackerWin ? "Win" : "Failed");
                msgTextBlock.Foreground = new SolidColorBrush(attacker.MainColor);
            }
            else if (msg != null)
            {
                Player player = playerManager.GetPlayer(stateManager.ActivePlayerId);
                msgTextBlock.Text = "Player " + player.PlayerId + ": \n" + msg;
                msgTextBlock.Foreground = new SolidColorBrush(player.MainColor);
            }
            else
            {
                msgTextBlock.Text = "";
            }

            if (msgTextBlock.Text.Length == 0)
            {
                msgTextBlock.Opacity = 0;
            }
            else
            {
                msgTextBlock.Opacity = 1;

                //animation
                double inTime = 0.5, holdTime = 4, outTime = 0.5;
                DoubleAnimation opacityInAnim = new DoubleAnimation(0, 1, new Duration(TimeSpan.FromSeconds(inTime)));
                DoubleAnimation motionInAnim = new DoubleAnimation(-20, 0, new Duration(TimeSpan.FromSeconds(inTime)));
                DoubleAnimation opacityOutAnim = new DoubleAnimation(1, 0, new Duration(TimeSpan.FromSeconds(outTime)))
                {
                    BeginTime = TimeSpan.FromSeconds(inTime + holdTime)
                };
                DoubleAnimation motionOutAnim = new DoubleAnimation(0, 10, new Duration(TimeSpan.FromSeconds(outTime)))
                {
                    BeginTime = TimeSpan.FromSeconds(inTime + holdTime)
                };
                motionOutAnim.Completed += new EventHandler(motionOutAnim_Completed);

                msgTextBlock.BeginAnimation(TextBlock.OpacityProperty, opacityInAnim, HandoffBehavior.SnapshotAndReplace);
                textMotionTrans.BeginAnimation(TranslateTransform.YProperty, motionInAnim, HandoffBehavior.SnapshotAndReplace);

                if (stateManager.State == GameState.AttackAnimation)
                {
                    msgTextBlock.BeginAnimation(TextBlock.OpacityProperty, opacityOutAnim, HandoffBehavior.Compose);
                    textMotionTrans.BeginAnimation(TranslateTransform.YProperty, motionOutAnim, HandoffBehavior.Compose);
                }
            }
            
        }

        void motionOutAnim_Completed(object sender, EventArgs e)
        {
            if (AnimationCompleted != null)
            {
                AnimationCompleted(this, EventArgs.Empty);
            }
        }
    }
}

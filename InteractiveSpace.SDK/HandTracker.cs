using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Media3D;

namespace InteractiveSpace.SDK
{
    public interface HandTracker
    {
        event EventHandler<HandEventArgs> HandCaptured;
        event EventHandler<HandEventArgs> HandLost;
        event EventHandler<HandEventArgs> HandMove;

        Hand FindHandById(int id);
        IEnumerable<Hand> Hands { get; }
        int HandCount { get; }
    }

    public class Hand
    {
        private int id;
        
        public Point3D Position {get; set;}
        public int ID { get {return id;}}

        public Hand(int id)
        {
            this.id = id;
        }

        public Hand(int id, Point3D position)
        {
            this.id = id;
            this.Position = position;
        }
    }

    public class HandEventArgs : EventArgs
    {
        private Hand hand;
        private HandTracker handTracker;

        public HandEventArgs(int id, Point3D position, HandTracker handTracker)
        {
            this.handTracker = handTracker;
            hand = new Hand(id, position);
        }

        public HandEventArgs(Hand hand, HandTracker handTracker)
        {
            this.handTracker = handTracker;
            this.hand = hand;
        }

        public int ID { get { return hand.ID; } }
        public Point3D Position { get { return hand.Position; } }
        public HandTracker HandTracker { get { return handTracker; } }
    }
}

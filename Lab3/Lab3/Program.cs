using System;
using System.Collections.Generic;

namespace Lab3
{
    class Program
    {
        static private void printQueue(List<Item> list)
        {
            double avg = 0;
            Console.WriteLine("The Queue item's time: ");
            foreach (var x in list)
            {
                Console.Write(x.MaxTime + " ");
                avg += x.MaxTime;
            }
            Console.WriteLine();
            Console.WriteLine("Average time of waiting: " + avg/list.Count);
            Console.WriteLine();
        }
        static void Main(string[] args)
        {
            QueueData qd = new QueueData();
            qd.addQueue(new Item(1,6));
            qd.addQueue(new Item(2,8));
            qd.addQueue(new Item(3,7));
            qd.addQueue(new Item(4,3));
            qd.addQueue(new Item(5,4));
            qd.addQueue(new Item(6,5));
            qd.addQueue(new Item(7,1));
            printQueue(qd.getQueue());
            qd.SortSJF();
            printQueue(qd.getQueue());
            qd.addQueue(new Item(8, 7));
            qd.addQueue(new Item(9, 3));
            qd.addQueue(new Item(10, 4));
            qd.addQueue(new Item(11, 5));
            printQueue(qd.getQueue());
            qd.SortSJF();
            printQueue(qd.getQueue());
        }
    }
}

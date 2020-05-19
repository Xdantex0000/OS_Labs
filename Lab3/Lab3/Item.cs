using System;
using System.Collections.Generic;
using System.Text;

namespace Lab3
{
    class Item
    {
        public int Number { get; set; }
        public int MaxTime { get; set; }

        public Item(int number, int time)
        {
            Number = number;
            MaxTime = time;
        }
    }
}

using System.Collections.Generic;
using System.Linq;

namespace Lab3
{
    class QueueData
    {
        Queue<Item> queue;

        public QueueData()
        {
            queue = new Queue<Item>();
        }

        public List<Item> getQueue()
        {
            int i = 0, temp = 0;
            List<Item> list = new List<Item>();
            foreach (var x in queue)
            {
                list.Add(new Item(x.Number,x.MaxTime));
            }
            foreach(var x in list)
            {
                temp = x.MaxTime;
                x.MaxTime = i;
                i += temp;
            }
            return list;
        }

        public void addQueue(Item item)
        {
            queue.Enqueue(item);
        }

        public void SortSJF()
        {
            var arr = queue.OrderBy(x => x.MaxTime).ToArray();
            queue = new Queue<Item>();

            foreach (var x in arr)
                addQueue(x);
            getQueue();
        }
    }
}

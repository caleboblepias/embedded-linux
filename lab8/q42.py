import threading
import logging
import queue
class item:
    def __init__(self, char, val):
        self.char = char
        self.val = val
        

class producerThread(threading.Thread):
    def __init__(self, queue, cond, name):
        super().__init__()
        self.queue = queue
        self.cond = cond
        self.name = name

    def run(self):
        for i in range(50):
            with self.cond:
                while self.queue.full():
                    self.cond.wait()
                self.queue.put(item(self.name, i))
                logging.debug('Added %s, %d', self.name, i)
                self.cond.notify()


class consumerThread(threading.Thread):
    def __init__(self, queue, cond):
        super().__init__()
        self.queue = queue
        self.cond = cond

    def run(self):
        for i in range(50):
            with self.cond:
                while self.queue.empty():
                    self.cond.wait()
                out = self.queue.get()
                logging.debug('Popped %s, %d', out.char, out.val)
                self.cond.notify()


logging.basicConfig(
    level=logging.DEBUG, format='(%(threadName)-10s) %(message)s',
)

q = queue.Queue()
c = threading.Condition()
p1 = producerThread(q, c, 'A')
c1 = consumerThread(q, c)
p1.start()
c1.start()

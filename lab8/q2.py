import threading
import logging

class MyThread(threading.Thread):
    def __init__(self, lockA, lockB, name):
        super().__init__();
        self.lockA = lockA
        self.lockB = lockB
        self.name = name
    def run(self):
        logging.debug('%s running', self.name)
        self.lockA.acquire()
        logging.debug('%s lockA acquired', self.name)
        self.lockB.acquire()
        logging.debug('%s lockB acquired', self.name)
        self.lockB.release()
        logging.debug('%s lockB released', self.name)
        self.lockA.release()
        logging.debug('%s lockA released', self.name)
        logging.debug('%s finished')

logging.basicConfig(
    level=logging.DEBUG,
    format='(%(threadName)-10s) %(message)s',
)

lockA = threading.Lock()
lockB = threading.Lock()
t1 = MyThread(lockA, lockB, "thread1")
t2 = MyThread(lockA, lockB, "thread2")

t1.start()
t2.start()



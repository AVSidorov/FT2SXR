from matplotlib import pyplot as plt
import numpy as np

muy = [5.0, 5.0, 3.0, 5.0, 4.5, 2.3, 3.2, 1.4]

prog = [0.5, 1.7, 1.4, 1.7, 1.7, 1.0, 1.3, 0.8]

eye = [0.9, 3.2, 1.9, 2.4, 2.1, 1.1, 2.4, 1.0]

plt.plot(prog, muy, 'or')
# plt.plot(eye, muy, '*b')
# plt.plot(np.linspace(0, 5, 30), np.linspace(0, 5, 30))
# plt.plot(np.linspace(0, 2.5, 30), np.linspace(0, 2.5, 30)*1.5)
plt.plot(np.linspace(0, 2, 30), np.linspace(0, 2, 30)*2.5)
plt.ylabel('Точный поток')
plt.xlabel('Примерный поток')
plt.legend(('Точный/примерный', 'Прямая 2.5:1'))
plt.grid()
plt.tight_layout()
plt.show()
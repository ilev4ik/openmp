z1_time = 9.3335
z2_time = 4.2840
thread_num = [i for i in range(2,size+2,1)]
acceleration = [z2_time/i for i in result]
efficiency = [a/n for (a,n) in zip(acceleration, thread_num)]
plt.plot(thread_num, efficiency)
plt.ylabel('efficiency')
plt.xlabel('thread num')
plt.show()
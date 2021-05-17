import os
from datetime import datetime

tasks = [
    ["A", "2017-02-01", "2017-03-01", "Sam", "Evan", "Troy"], 
    ["B", "2017-01-16", "2017-02-15", "Troy"], 
    ["C", "2017-02-13", "2017-03-13", "Sam", "Evan"]
  ]

queries = [["Evan", "2017-03-10"], ["Troy", "2017-02-04"]]

answer = []

tasks_d = [{'name':a[0], 'start': datetime.strptime(a[1], '%Y-%m-%d').date(),
  'end': datetime.strptime(a[2], '%Y-%m-%d').date(), 'assignees': a[3:]} for a in tasks]


for q in queries:
  assignee = q[0]
  d = datetime.strptime(q[1], '%Y-%m-%d').date()
  tk = []
  for t in tasks_d:
    for p in t['assignees']:
      if assignee == p and d >= t['start'] and d <= t['end']:
        tk.append([t['name'], t['end']])
  
  tk = sorted(tk, key=lambda x: (x[1], x[0]), reverse=True)
  l1 = []
  for t1 in tk:
    l1.append(t1[0])
  answer.append(l1)

print(answer)
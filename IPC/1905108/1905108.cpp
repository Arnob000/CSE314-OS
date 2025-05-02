#include<bits/stdc++.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<ctime>
#include<random>

using namespace std;

#define PRINTERS 4

#define INIT 0
#define WAITING 1
#define PRINTING 2
#define PRINTDONE 3

int n,m,w,x,y;
pthread_t th[2000];
pthread_t r[2];
int reader=0;

class Student{
public:
    int id;
    int state;
    int groupNo;
    Student(int i){
        id=i;
        state=INIT;
    }
};

vector<Student* > pr1;
vector<Student* > pr2;
vector<Student* > pr3;
vector<Student* > pr4;
vector<int> prdone;

time_t startTime;
time_t currTime;

default_random_engine g;
//mt19937 gen(g());
mt19937 gen(time(NULL));
poisson_distribution<int> pRand(6);

//printing
sem_t stuPrint[2000];
sem_t prMutex[PRINTERS];
sem_t grpPrint[2000];

//binding
sem_t binding;
sem_t grpBind[2000];

//submission
int submission=0;
sem_t rdSub;
sem_t wrtSub;

//console
sem_t conlock;


void init_semaphore()
{
	sem_init(&wrtSub,0,1);
	sem_init(&rdSub,0,1);

//	sem_init(&prMutex,0,1);
	sem_init(&binding,0,2);

	sem_init(&conlock,0,1);

	for(int i=1; i<=n; i++){
        sem_init(&stuPrint[i],0,0);
    }

    for(int i=1; i<=(n/m); i++){
        sem_init(&grpPrint[i],0,1);
        sem_init(&grpBind[i],0,0);
	}

	for(int i=0; i<PRINTERS; i++){
        sem_init(&prMutex[i],0,1);
	}
}


void test(Student* s){
    vector<Student*> pr;
    pr.clear();
    int flag=0;

    if((s->id%PRINTERS)+1 == 1){
        for(int i=0; i<pr1.size(); i++){
            pr.push_back(pr1[i]);
        }
    }
    else if((s->id%PRINTERS)+1 == 2){
        for(int i=0; i<pr2.size(); i++){
            pr.push_back(pr2[i]);
        }
    }
    else if((s->id%PRINTERS)+1 == 3){
        for(int i=0; i<pr3.size(); i++){
            pr.push_back(pr3[i]);
        }
    }
    else if((s->id%PRINTERS)+1 == 4){
        for(int i=0; i<pr4.size(); i++){
            pr.push_back(pr4[i]);
        }
    }

    for(int i=0; i<pr.size(); i++){
        //cout<<pr[i]->id<<" "<<pr[i]->state<<endl;
        if(pr[i]->state == PRINTING)
            flag=1;
    }

    if(s->state == WAITING && flag == 0){
        s->state = PRINTING;
        sem_post(&stuPrint[s->id]);
    }
}

void takePrinter(Student* s){
    sem_wait(&prMutex[s->id%PRINTERS]);
    s->state=WAITING;

    if((s->id%PRINTERS)+1 == 1)
        pr1.push_back(s);
    else if((s->id%PRINTERS)+1 == 2)
        pr2.push_back(s);
    else if((s->id%PRINTERS)+1 == 3)
        pr3.push_back(s);
    else if((s->id%PRINTERS)+1 == 4)
        pr4.push_back(s);

    test(s);
    sem_post(&prMutex[s->id%PRINTERS]);
    sem_wait(&stuPrint[s->id]);
}

void releasePrinter(Student* s){
    vector<Student*> pr;
    sem_wait(&prMutex[s->id%PRINTERS]);
    s->state=PRINTDONE;

    if((s->id%PRINTERS)+1 == 1){
        for(int i=0; i<pr1.size(); i++){
            pr.push_back(pr1[i]);
        }
    }
    else if((s->id%PRINTERS)+1 == 2){
        for(int i=0; i<pr2.size(); i++){
            pr.push_back(pr2[i]);
        }
    }
    else if((s->id%PRINTERS)+1 == 3){
        for(int i=0; i<pr3.size(); i++){
            pr.push_back(pr3[i]);
        }
    }
    else if((s->id%PRINTERS)+1 == 4){
        for(int i=0; i<pr4.size(); i++){
            pr.push_back(pr4[i]);
        }
    }

    for(int i=0; i<pr.size(); i++){
        if(pr[i]->groupNo == s->groupNo)
            test(pr[i]);
    }

    for(int i=0; i<pr.size(); i++){
        if(pr[i]->groupNo != s->groupNo)
            test(pr[i]);
    }

    sem_post(&prMutex[s->id%PRINTERS]);
}

void* stuThread(void* arg){
    int delay=pRand(gen);
    sleep(delay);

    Student* s=(Student*)arg;

    //    printing
    sem_wait(&conlock);
    currTime=time(NULL);
    cout<<"Student "<<s->id<<" has arrived at printing station "<<((s->id%PRINTERS)+1)<<" at time "<<(currTime-startTime)<<endl;
    sem_post(&conlock);

    takePrinter(s);

    sem_wait(&conlock);
    currTime=time(NULL);
    cout<<"Student "<<s->id<<" has started printing at printing station "<<((s->id%PRINTERS)+1)<<" at time "<<(currTime-startTime)<<endl;
    sem_post(&conlock);

    sleep(w);

    sem_wait(&conlock);
    currTime=time(NULL);
    cout<<"Student "<<s->id<<" has finished printing at time "<<(currTime-startTime)<<endl;
    sem_post(&conlock);

    releasePrinter(s);

    sem_wait(&grpPrint[s->groupNo]);
    prdone[s->groupNo]++;

    if(prdone[s->groupNo]==m){
        sem_post(&grpBind[s->groupNo]);
    }
    sem_post(&grpPrint[s->groupNo]);

    if(s->id%m != 0) //only leader remains
        return 0;

    //binding
    sem_wait(&grpBind[s->groupNo]);

    sem_wait(&conlock);
    currTime=time(NULL);
    cout<<"Group "<<s->groupNo<<" has finished printing at time "<<(currTime-startTime)<<endl;
    sem_post(&conlock);

    delay=pRand(gen)%7;
    sleep(delay);

    sem_wait(&binding);

    sem_wait(&conlock);
    currTime=time(NULL);
    cout<<"Group "<<s->groupNo<<" has started binding at time "<<(currTime-startTime)<<endl;
    sem_post(&conlock);

    sleep(x);

    sem_wait(&conlock);
    currTime=time(NULL);
    cout<<"Group "<<s->groupNo<<" has finished binding at time "<<(currTime-startTime)<<endl;
    sem_post(&conlock);

    sem_post(&binding);

    //submitting
    delay=pRand(gen)%17;
    sleep(delay);

    sem_wait(&wrtSub);

    sem_wait(&conlock);
    currTime=time(NULL);
    cout<<"Group "<<s->groupNo<<" has started submitting at time "<<(currTime-startTime)<<endl;
    sem_post(&conlock);

    sleep(y);
    submission++;

    sem_wait(&conlock);
    currTime=time(NULL);
    cout<<"Group "<<s->groupNo<<" has finished submitting at time "<<(currTime-startTime)<<endl;
    sem_post(&conlock);

    sem_post(&wrtSub);

    pthread_exit(NULL);
}

void* readerThread(void* arg){
    int delay;
    int sub;
    long long rid=(long long)arg;
    while(true){
        delay=pRand(gen);
        sleep(delay);

        sem_wait(&rdSub);
        reader++;
        if(reader==1)
            sem_wait(&wrtSub);
        sem_post(&rdSub);

        sub=submission;
        sem_wait(&conlock);
        currTime=time(NULL);
        cout<<"Staff "<<rid<<" has started reading the entry book at time "<<currTime-startTime;
        cout<<". No of submission = "<<submission<<endl;
        sem_post(&conlock);

        sleep(y);

        sem_wait(&rdSub);
        reader--;
        if(reader==0)
            sem_post(&wrtSub);
        sem_post(&rdSub);

        if(n/m == sub){
            pthread_exit(NULL);
        }
    }
}


int main(){
    freopen("input.txt","r",stdin);
    freopen("output.txt","w",stdout);
    startTime=time(NULL);
    cin>>n>>m;
    cin>>w>>x>>y;

    long long staff1 = 1;
	long long staff2 = 2;

	init_semaphore();

    pthread_create(&r[0], NULL, readerThread, (void*)staff1);
    pthread_create(&r[1], NULL, readerThread, (void*)staff2);

    Student* s;
    for(int i=1; i<=n; i++){
        s=new Student(i);
        s->groupNo=((i-1)/m)+1;
        prdone.push_back(0);
        pthread_create(&th[i], NULL, stuThread,(void *) s);
    }

    pthread_exit(NULL);
    return 0;
}

function [Peaks,CountRate]=CountExpPulses(S,AverageTime,MinFront,MinAmpl,StartInd,PlotOn)
% coarse counting SDD pulses with exponential tails
% S - signal from SDD with positive response pulses   
% AverageTime - time unterval for calculation of the count rate 
% MinFront - the least front edge, tacts
% MinAmpl - the least peak amplitude
% StartInd - the index of S when real signal can start, in tacts
% Peaks - columns of top pulse indexes, pulse amplitudes, pulse rise times and reciprical  
% CountRate - the number of pulses coounted for AverageTime (us) interval

Tact=0.004; % us, sampling tact of signal

if nargin<2; AverageTime=100; end; % us, time interval for counting pulse number
if nargin<3; MinFront=10; end; 
if nargin<4; MinAmpl=500; end; 
if nargin<5; StartInd=3e6;  end;
if nargin<5; PlotOn=1; end; 

AverageTactN=fix(AverageTime/Tact); 

SN=length(S); 
t=[1:SN]'*Tact/1000;  % ms
% neighbour signals from the right and left hand sides: 
SR=[S(2:end);S(end)]; SL=[S(1);S(1:end-1)];
% neighbour but on signals from the right and left hand sides: 
SR2=[SR(2:end);SR(end)]; SL2=[SL(1);SL(1:end-1)];
% indexes of signal tops and downs:
TopInd=find(S>SL & S>SL2 & S>=SR & S>=SR2);
DownInd=find(S<SL & S<SL2 & S<=SR & S<=SR2);
% Combine tops and downs:  
TopInd(:,2)=1; DownInd(:,2)=0;
TopDownInd=[TopInd;DownInd]; 
[Sort,SortInd]=sort(TopDownInd(:,1)); TopDownInd=TopDownInd(SortInd,:);
% start TopDownInd from the first down:
ind=find(TopDownInd(:,2)==0,1,'first'); 
if ind>1; TopDownInd(1:ind-1,:)=[]; end; 

% redefine signal tops and downs: 
TopInd=find(TopDownInd(:,2)==1);
TopIndN=length(TopInd); 
DownInd=zeros(size(TopInd)); 

ind0=1; 
for i=1:TopIndN
    ind=find(TopDownInd(ind0:TopInd(i),2)==0); 
    indN=length(ind); 
    switch indN
        case 0 
         [Min,IndD]=min(S(TopDownInd(ind0:TopInd(i),1)));   
         DownInd(i)=IndD+ind0-1; 
        otherwise 
         DownInd(i)=ind(end)+ind0-1;    
    end;
    ind0=TopInd(i);
end; 
TopInd(:,2)=TopDownInd(TopInd(:,1),1);
DownInd(:,2)=TopDownInd(DownInd(:,1),1);
TopInd(:,3)=S(TopInd(:,2));
DownInd(:,3)=S(DownInd(:,2)); 
Peaks=[TopInd(:,2),TopInd(:,3)-DownInd(:,3),TopInd(:,2)-DownInd(:,2)]; 
Peaks(Peaks(:,2)<=0,:)=[]; 
Peaks(Peaks(:,3)<=MinFront,:)=[]; Peaks(:,3)=Peaks(:,3)*Tact;
Peaks(Peaks(:,2)<=MinAmpl,:)=[]; 
Peaks(Peaks(:,1)<=StartInd,:)=[]; 
% Tact number from the preceeding pulse:
Peaks(2:end,4)=diff(Peaks(:,1)); Peaks(1,4)=mean(Peaks(2:end,4));

% The number of pulses in AverageTime
IntervalsN=fix((Peaks(end,1)-Peaks(1,1))/AverageTactN);
CountRate=zeros(IntervalsN,2); 
ind0=Peaks(1,1);
for i=1:IntervalsN
    ind1=ind0+AverageTactN*(i-1); 
    ind2=ind1+AverageTactN-1; 
    ind=find(Peaks(:,1)>=ind1 & Peaks(:,1)<=ind2); 
    indN=length(ind);
    CountRate(i,2)=indN; 
    if indN>0; CountRate(i,1)=mean(Peaks(ind,1)); end;     
end; 
CountRate(CountRate(:,1)==0,:)=[]; 
CountRate(:,1)=CountRate(:,1)*Tact/1000; % ms
CountRate(:,2)=CountRate(:,2)/AverageTime;  % 1/us
if PlotOn
    figure;
    % signals and peaks:
    subplot(2,1,1); hold on; plot(t,S,'b'); plot(Peaks(:,1)*Tact/1000,Peaks(:,2),'r.'); grid on;
    % plot(TopInd(:,2),TopInd(:,3),'m.'); plot(DownInd(:,2),DownInd(:,3),'c.');
    xlabel('t, ms');  ylabel('Pulse ampl');
    
    subplot(2,1,2); hold on;
    plot(Peaks(:,1)*Tact/1000,1./Peaks(:,4)/Tact,'r.');   grid on;
    plot(CountRate(:,1),CountRate(:,2),'b','Linewidth',2);
    xlabel('t, ms'); ylabel('Counts/\mus');
end;





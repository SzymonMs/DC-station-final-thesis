%% Wczytanie danych
testU = load('pomiarU.csv');
testW = load('pomiarW.csv');
u = testU(:,2);
u = u(374:size(u,1));
w = testW(:,2);
w = w(374:size(w,1));
w = 2*pi/60*w;
sizeW = size(w,1);
sizeU = size(u,1);
uEst = u(1:floor(sizeU/2));
uWer = u(floor(sizeU/2)+1:sizeU);
wEst = w(1:floor(sizeW/2));
wWer = w(floor(sizeW/2)+2:sizeW);
Tp = 0.01;
N = size(wEst,1);
t=(0:Tp:(N-1)*Tp)';
%% Identyfikacja LS
u = uEst;
y = wEst;
fi = [];
FI = [];
dp =3;
FI(1,:) = [0,0,0];
FI(2,:) = [0,0,0];
for i = dp:size(y,1)
   fi = [y(i-1),y(i-2),u(i-2)];
   FI(i,:) = fi;
   fi = [];
end
parm = inv(FI'*FI)*FI'*y;
fup = [];
fup(1) = 0;
fup(2) = 0;
for i = dp:size(y,1)
    fup(i) = parm(1)*fup(i-1)+parm(2)*fup(i-2)+parm(3)*u(i-2);
end
plot(t,y,t,fup)

%% Metoda IV
z = [];
Z = [];
Z(1,:)=[0,0,0];
Z(2,:)=[0,0,0];
for i = dp:size(y,1)
    z = [fup(i-1),fup(i-2), u(i-2)];
    Z(i,:) = z;
    z = [];
end
parm2 = inv(Z'*FI)*Z'*y;
for i = dp:size(y,1)
    fup(i) = parm2(1)*fup(i-1)+parm2(2)*fup(i-2)+parm2(3)*u(i-2);
end
plot(t,y,t,fup)
%%
wer = [];
wer(1) = 0;
wer(2) = 0;
wer2 = [];
wer2(1) =wWer(1);
wer2(2) = wWer(2);
%weryfikacja modelu z IV
for i = dp:size(y,1)
    wer(i) = parm2(1)*wer(i-1)+parm2(2)*wer(i-2)+parm2(3)*uWer(i-2);
end
%Sprawdzanie pomiarowej
Ra = 0.3;
La = 0.001352;
ka = 0.1266;
Ja = 0.00072;
p11 = (2*La-Ra*Tp)/La;
p22 = (Ra*Ja*Tp-La*Ja-ka^2*Tp^2)/(La*Ja);
p33 = ka*Tp^2/(La*Ja);
%weryfikacja modelu z identyfikacji pomiarowej
for i = dp:size(y,1)
    wer2(i) = p11*wer2(i-1)+p22*wer2(i-2)+p33*uWer(i-2);
end
plot(t,wWer,t,wer,t,wer2)
xlabel('t[s]')
ylabel('w[rad/s]')
title('Weryfikacja parametrów')
legend('Sygna³ rzeczywisty','Sygna³ zamodelowany','Sygna³ z modelu pomiarowego')
% plot(t,wWer,t,wer)
% xlabel('t[s]')
% ylabel('w[rad/s]')
% title('Weryfikacja parametrów')
% legend('Sygna³ rzeczywisty','Sygna³ zamodelowany')
%% Równanie
R = 0.3;
syms L J k
eqns = [(2*L-R*Tp)/L == parm2(1) , (R*J*Tp-L*J-k^2*Tp^2)/(L*J) == parm2(2), k*Tp^2/(L*J) == parm2(3)];
S = solve(eqns,[L J k])
L=double(S.L)
k=double(S.k)
J=double(S.J)
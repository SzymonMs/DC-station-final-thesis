clear all
clc
close all
% Delete Opened Ports in MATLAB
delete(instrfind)
% Create a Serial Object
ser = serial('COM4', 'BaudRate',115200, 'Timeout',10);

DATA_SIZE = 1000;
temperature = zeros(DATA_SIZE, 1);
temperature2 = zeros(DATA_SIZE, 1);
timestamp = zeros(DATA_SIZE, 1);
timestamp = datetime(timestamp, timestamp, timestamp);
for i = 1:DATA_SIZE
   timestamp(i) = datetime;
end
index = 1;

% Open Communication Port
fopen(ser);
while true
    temp = fscanf(ser);
    temp=split(temp," ");
    if size(temp) == [0][0]
        disp 'Timeout Occurs'
        disp 'Check Connections'
        break
    else
        if index < DATA_SIZE
            index = index+1;
        else
            % Reset Subscript Index
            index = 1;
        end
        temperature(index) = str2double(temp(1,1));
        temperature2(index) = str2double(temp(2,1));
        % Time Stamp Temperature Values
         timestamp(index) = datetime;
         plot(timestamp, temperature,'*', 'LineWidth',2,'Color',[0,0,1.0])
         hold on
         plot(timestamp, temperature2,'.', 'LineWidth',2,'Color',[1.0,0,0])
         hold off
%         plot(temperature,'.')
%         hold on
%         plot(temperature2,'.')
%         hold off
        drawnow
    end
end

fclose(ser);
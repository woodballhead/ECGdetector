clear all
close all
clc
ecg = load('F:\工作\手机外壳算法\ECGdetector\Data\ecg.txt');
ecg_filtered = load('F:\工作\手机外壳算法\ECGdetector\Data\ecg_filtered.txt');


plot(ecg_filtered),grid
hold on
result =  load('F:\工作\手机外壳算法\ECGdetector\Data\ecg_out.txt');
rlist = result(:,2);
plot(rlist,ecg_filtered(rlist),'mo')
plot(1:2:2*length(ecg),ecg,'g')


% ecg = [ecg;ecg;ecg;ecg;ecg];
% fp = fopen('F:\工作\手机外壳算法\ECGdetector\Data\ecg.txt', 'w');
% for i=1:size(ecg,1)
%     fprintf(fp, '%d\n',ecg(i));
% end
% fclose(fp);
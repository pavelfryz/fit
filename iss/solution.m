src = 'xfryzp00.bmp';
img = imread(src);
file = fopen('reseni.txt','w','n','US-ASCII');
% zaosterni obrazu
h = [-0.5 -0.5 -0.5;-0.5 5.0 -0.5;-0.5 -0.5 -0.5];
img = imfilter(img,h);
imwrite(img,'step1.bmp');
% otoceni obrazu
img = fliplr(img);
imwrite(img,'step2.bmp');
% medianovy filt
img = medfilt2(img, [5 5]);
imwrite(img,'step3.bmp');
% rozmazani obrazu
h = [1 1 1 1 1;1 3 3 3 1;1 3 9 3 1;1 3 3 3 1;1 1 1 1 1]/49;
img = imfilter(img,h);
imwrite(img,'step4.bmp');
% chyba v obraze
ref = imread(src);
ref = fliplr(ref);
imgd = im2double(img);
refd = im2double(ref);
x = mean2(abs(imgd-refd))*255;
fprintf(file,'chyba=%f\n',x);
% roztazeni histogramu
mini = min(min(imgd));
maxi = max(max(imgd));
imgdhist = (imgd-mini).*(1/(maxi-mini));
mini = min(min(imgdhist));
maxi = max(max(imgdhist));
img = im2uint8(imgdhist);
imwrite(img,'step5.bmp');
% spocitani stredni hodnoty a smerodatne odchylky
x = mean2(imgd)*255;
fprintf(file,'mean_no_hist=%f\n',x);
x = std2(imgd)*255;
fprintf(file,'std_no_hist=%f\n',x);
x = mean2(imgdhist)*255;
fprintf(file,'mean_hist=%f\n',x);
x = std2(imgdhist)*255;
fprintf(file,'std_hist=%f\n',x);
% kvantizace obrazu
imgdkvant = round(imgdhist.*3)/3;
img = im2uint8(imgdkvant);
imwrite(img,'step6.bmp');


% Display 2-D Image

function DisplayImage2D(imgdata,DisplayScale,ImgTitleName)

figure
imagesc(imgdata,DisplayScale);
colormap(gray);

color_bar = colorbar;
DensityTickLabel = char(get(color_bar,'YTickLabel'));
mm = repmat(' mm^-1',size(DensityTickLabel,1),1);
DensityTickLabel = [DensityTickLabel mm];
set(color_bar,'YTickLabel',DensityTickLabel);

title(ImgTitleName);
axis('image')
xlabel('X');
ylabel('Y');
end
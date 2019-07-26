% Display 2-D Image

function [fig] = DisplayImage2D(imgdata,DisplayParams)

fig=figure;
set(fig, 'Visible', 'off');
imagesc(imgdata,DisplayParams.DisplayScale);
colormap(gray);

% Color bar with units
color_bar = colorbar;
DensityTickLabel = char(get(color_bar,'YTickLabel'));
mm = repmat(' mm^{-1}',size(DensityTickLabel,1),1);
DensityTickLabel = [DensityTickLabel mm];
set(color_bar,'YTickLabel',DensityTickLabel);

xlabel(DisplayParams.XAxisLabel);
ylabel(DisplayParams.YAxisLabel);
title(DisplayParams.Title,'Interpreter','LaTex');


end
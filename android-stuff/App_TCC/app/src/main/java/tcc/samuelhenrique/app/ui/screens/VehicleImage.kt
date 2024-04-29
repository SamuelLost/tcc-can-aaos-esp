package tcc.samuelhenrique.app.ui.screens

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.size
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.Dp
import coil.compose.rememberAsyncImagePainter
import coil.request.ImageRequest
import tcc.samuelhenrique.app.R
import tcc.samuelhenrique.app.acc_fault
import tcc.samuelhenrique.app.temperature_fault

@Composable
fun VehicleImage(tempFault: Boolean, accFault: Boolean, heightImg: Dp, widthImg: Dp) {
    Image(
        painter = painterResource(
            id = when {
                temperature_fault && acc_fault -> R.drawable.carro_acc_temp_v3
                temperature_fault -> R.drawable.carro_temp_v3
                acc_fault -> R.drawable.carro_acc_v3
                else -> R.drawable.carro_v3
            }
        ),
        contentDescription = "",
        modifier = Modifier.size(
            width = widthImg,
            height = heightImg
        )
    )
}

@Composable
fun VehicleImageSvgFromAssets(tempFault: Boolean, accFault: Boolean, heightImg: Dp, widthImg: Dp) {
    val painter = rememberAsyncImagePainter(
        model = ImageRequest.Builder(LocalContext.current)
            .data(
                when {
                    temperature_fault && acc_fault -> "file:///android_asset/carro_acc_temp.svg"
                    temperature_fault -> "file:///android_asset/carro_temp.svg"
                    acc_fault -> "file:///android_asset/carro_acc.svg"
                    else -> "file:///android_asset/carro.svg"
                }
            )
            .build()
    )

    Image(
        painter = painter,
        contentDescription = "",
        modifier = Modifier.size(
            width = widthImg,
            height = heightImg
        )
    )
}
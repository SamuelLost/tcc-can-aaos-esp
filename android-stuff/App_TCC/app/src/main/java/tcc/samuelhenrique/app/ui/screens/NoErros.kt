package tcc.samuelhenrique.app.ui.screens

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import tcc.samuelhenrique.app.R
import tcc.samuelhenrique.app.ui.theme.Subtitulos

@Composable
fun NoErrors() {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Icon(
            imageVector = Icons.Default.CheckCircle,
            contentDescription = "No errors",
            tint = Color(0xff196628),
            modifier = Modifier
                .align(Alignment.CenterHorizontally)
                .size(64.dp)
        )

        Text(text = "Está tudo bem com seu veículo",
            color = Subtitulos,
            fontSize = 18.sp,
            fontFamily = FontFamily(
                Font(R.font.poppins_bold)
            ),
            modifier = Modifier.padding(top = 52.dp)
        )

    }
}

@Preview
@Composable
fun PreviewNoErrors() {
    NoErrors()
}
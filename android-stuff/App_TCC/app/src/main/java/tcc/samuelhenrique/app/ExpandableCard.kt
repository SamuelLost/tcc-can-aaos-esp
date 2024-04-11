package tcc.samuelhenrique.app

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.KeyboardArrowDown
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.rotate
import androidx.compose.ui.draw.shadow
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import tcc.samuelhenrique.app.ui.theme.Erro_Vermelho
import tcc.samuelhenrique.app.ui.theme.Texto

@Composable
fun ExpandableCard(
    titleCard: String,
    description: @Composable () -> Unit,
    gravity: @Composable () -> Unit,
    currentReading: @Composable () -> Unit
) {
    var expanded by remember { mutableStateOf(false) }
    val rotateState by animateFloatAsState(targetValue = if (expanded) 180f else 0f, label = "")

    Card(
        colors = CardDefaults.cardColors(
            containerColor = Erro_Vermelho,
            contentColor = Texto
        ),
        shape = RoundedCornerShape(16.dp),
        elevation = CardDefaults.cardElevation(
            defaultElevation = 16.dp
        ),
        modifier = Modifier
//            .clip(RoundedCornerShape(16.dp)) // O efeito de ripple respeita o shape do card
//            .clickable(onClick = { expanded = !expanded })
            .clickable(
                interactionSource = remember { MutableInteractionSource() },
                indication = null
            ) { expanded = !expanded } // Remove o efeito de click do card
            .width(450.dp)
            .height(if (expanded) 288.dp else 48.dp)

    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(12.dp)
        ) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Text(
                    modifier = Modifier.weight(6f),
                    text = titleCard,
                    fontSize = 18.sp,
                    fontFamily = FontFamily(
                        Font(R.font.poppins_bold)
                    ),
                    color = Texto,
                    maxLines = 1,
                    overflow = TextOverflow.Ellipsis
                )
                IconButton(
                    onClick = {
                        expanded = !expanded
                    },
                    modifier = Modifier.rotate(rotateState)
//                        .alpha(0.5f)
//                    modifier = Modifier.weight(1f)
                ) {
                    Icon(
                        imageVector = Icons.Default.KeyboardArrowDown,
                        contentDescription = "Drop-Down Arrow",
                        tint = Texto
                    )
                }
            }
//            if(expanded) {
            AnimatedVisibility(visible = expanded) {
                Column {
                    description()
                    Spacer(modifier = Modifier.padding(12.dp))
                    Row {
                        gravity()
                        Spacer(modifier = Modifier.padding(20.dp))
                        currentReading()
                    }
                }
            }
        }
    }
}

@Composable
fun Description(content: String) {
    Row {
        Text(
            text = content,
            fontSize = 16.sp,
            fontFamily = FontFamily(
                Font(R.font.poppins_regular)
            ),
            color = Texto
        )
    }
}

@Composable
fun Gravity(content: String) {
    Row {
        Box(
            modifier = Modifier
                .align(Alignment.CenterVertically)
                .height(120.dp)
                .width(180.dp)
                .shadow(1.dp, spotColor = Texto)
        ) {
            Text(
                text = content,
                fontSize = 28.sp,
                fontFamily = FontFamily(
                    Font(R.font.poppins_bold)
                ),
                color = Texto,
                modifier = Modifier
                    .padding(top = 18.dp)
                    .align(Alignment.TopCenter),
                maxLines = 1,
                overflow = TextOverflow.Ellipsis
            )
            Text(
                text = "Gravidade",
                fontSize = 16.sp,
                fontFamily = FontFamily(
                    Font(R.font.poppins_regular)
                ),
                color = Texto,
                modifier = Modifier
                    .padding(bottom = 9.dp)
                    .align(Alignment.BottomCenter),
                maxLines = 1,
                overflow = TextOverflow.Ellipsis
            )
        }
    }
}

@Composable
fun CurrentReading(content: String) {
    Row {
        Box(
            modifier = Modifier
                .align(Alignment.CenterVertically)
                .height(120.dp)
                .width(180.dp)
                .shadow(1.dp, spotColor = Texto)
        ) {
            Text(
                text = content,
                fontSize = 28.sp,
                fontFamily = FontFamily(
                    Font(R.font.poppins_bold)
                ),
                color = Texto,
                modifier = Modifier
                    .padding(top = 18.dp)
                    .align(Alignment.TopCenter),
//                maxLines = 1,
                overflow = TextOverflow.Ellipsis
            )
            Text(
                text = "Leitura Atual",
                fontSize = 16.sp,
                fontFamily = FontFamily(
                    Font(R.font.poppins_regular)
                ),
                color = Texto,
                modifier = Modifier
                    .padding(bottom = 9.dp)
                    .align(Alignment.BottomCenter),
                maxLines = 1,
                overflow = TextOverflow.Ellipsis
            )
        }
    }
}

@Preview(showBackground = true)
@Composable
fun ExandableCardPreview2() {
    ExpandableCard(titleCard = "TMP-01",
        description = { Description("Erro ao tentar conectar com o servidor") },
        gravity = { Gravity("Média") },
        currentReading = { CurrentReading("404") })

}